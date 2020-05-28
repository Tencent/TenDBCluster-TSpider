# Fast Deploy TenDBCluster Using Ansible

## 1. Prerequisites

Please refer to [Ansible Playbook Instructions](ansible-def-inventory-vars-en.md) for definitions of `host inventory` and `group_vars`.

### 1.1 Packages

Our Ansible playbook downloads packages via Http Get. If your hosts can access GitHub, you can assign `fileserver` to a GitHub address; if your hosts are in a private network, you need to construct a http file server.

Required Packages:
```
mysql-5.7.20-linux-x86_64-tmysql-3.1.5-gcs.tar.gz
mariadb-10.3.7-linux-x86_64-tspider-3.4.5-gcs.tar.gz
tdbctl-1.4-linux-x86_64.tar.gz
```

### 1.2 Ansible Manager Node

Download and install `ansible-2.6.13-1.el6.noarch.rpm`. Also we suggest Ansible version is between 2.4 and 2.9).

### 1.3 Configure SSH Communication

Currently we use a root user account install and maintain TenDBCluster.

#### Generate Public-Private Key Pair

Use command
```
ssh-keygen -t rsa -b 2048 -f tendbcluster_rsa
```
to generate a private key *tendbcluster_rsa* and a public key *tendbcluster_rsa.pub* under your current directory.

You can choose to manually distribute your ssh keys---in this case, skip this subsection. Alternatively, you can use the playbook *add_sshkey.yml* and add your public key to root, provided your root password.

#### Distribute Public key

In the Ansible Inventory, set the `ansible_ssh_public_key_file` variable (this is our customized variable instead of an Ansible built-in variable).
```
ansible_ssh_user=root
ansible_ssh_port=22
# ansible_ssh_private_key_file=/root/.ssh/tendbcluster_rsa

ansible_ssh_public_key_file=/root/.ssh/tendbcluster_rsa.pub
```

Here we use root to distribute the public key. We suggest using a same root password on all the hosts for convenience purpose (if you do not need to spare the trouble, you can add the `ansible_ssh_pass` variable to every host).
```
ansible-playbook -i hosts.tendbcluster add_sshkey.yml -u root -k
```
The `-k` option prompts input for root password. Note that the `ansible_ssh_private_key_file` needs to be commented at this step so that Ansible will not try to use it for login later.

#### After Public Key Distribution

1. Now you can change the root password on the hosts.
2. After the `ansible_ssh_private_key_file` variable is set and enabled, subsequent operations will be carried out via the private key instead of prompting for root password.

```
ansible_ssh_user=root
ansible_ssh_port=22
ansible_ssh_private_key_file=/root/.ssh/tendbcluster_rsa

ansible_ssh_public_key_file=/root/.ssh/tendbcluster_rsa.pub
```

### 1.4 MySQL Password Encryption

For security reasons, we use `ansible-vault` to encrypt passwords for MySQL admin/replication users. We set a password for both encrypting and decrypting MySQL users' passwords:
```
echo 'VaultPass123' > password_file
```

Generate ciphers:
```
# Generate MySQL root/admin password
ansible-vault encrypt_string --vault-id mysql@password_file xxxxxx --name tendbcluster_user_admin_pass

# Generate MySQL replication password
ansible-vault encrypt_string --vault-id mysql@password_file xxxxxx --name tendbcluster_user_repl_pass
```

Take the passwords generated above and respectively assign to the `tendbcluster_user_admin_pass` and `tendbcluster_user_repl_pass` variable in *group_vars/all*.

If you encounter Error *Attempting to decrypt but no vault secrets found* when executing playbooks in the follow-up steps, it means you need to append the `--vault-id password_file` option to your commands.

If you are in for convenience, you can just set the `tendbcluster_user_admin_pass` and `tendbcluster_user_repl_pass` to plaintext so that you do not need to specify `vault-id` every time you run playbooks.

## 2. Deploy TenDBCluster

### 2.1 Initialize Environment

```
ansible-playbook -i hosts.tendbcluster init_common.yml
```

You need to perform host initialization every time a new host joins.

The operations during initialization include:
- Modifying `nofile` in *limits.conf*;
- Disabling `SELinux`;
- Disabling `HugePage`;
- Setting `vm.swappiness=1`;
- Setting other variables like `ip_local_port_range` and `tcp_tw_recycle` etc.

Additionally, it:
- Creates essential MySQL users;
- Gathers physical memory size from machines (because using Ansible's built-in `facts` on a Docker container to do so may lead to wrong value).

### 2.2 Deploy TenDB

```
ansible-playbook -i hosts.tendbcluster install_tendb.yml
ansible-playbook -i hosts.tendbcluster build_slave.yml

# if you set mysql password encrypted:
ansible-playbook -i hosts.tendbcluster install_tendb.yml --vault-id password_file
ansible-playbook -i hosts.tendbcluster build_slave.yml --vault-id password_file
```
This two playbooks will check if the instance is already runing. You may need to add `-e force=true` to re-run the playbooks when failed.

*install_tendb.yml* mainly does:
- Install TenDB masters/slaves and start TenDB nodes;
- Generate *my.cnf* file, including calculation of `server_id` and `buffer pool`;
- Grant privileges for user admin/replication user to access within the cluster, using registered ip addresses in the host inventory.

*build_slave.yml* builds slaves in accordance with roles and masters, which mainly does:
- Create backups for tables on masters via `mysqldump --single-transaction`;
- Copy backups to slaves via `scp -3` on a Tdbctl node;
- Restore data from TenDB slave side via `mysql` client;
- Build the mater-slave replication with GTID mode by running `change master`.

You can rewrite the playbook to construct slaves based on your own interest or situation. For instance, you can pull physical backups from a cold-backup site and build slaves, as long as the configuration in the host inventory is set up correctly.

### 2.3 Deploy TSpider

```
ansible-playbook -i hosts.tendbcluster install_tspider.yml
```

*install_tspider.yml* mainly does:
- Install and start TSpider nodes;
- Generate *my.cnf* file, including auto generation of `autoinc_value`;
- Grant privileges to access within the cluster.

### 2.4 Deploy Tdbctl

```
ansible-playbook -i hosts.tendbcluster install_tdbctl.yml
```

*install_tdbctl.yml* mainly does:
- Install and start Tdbctl nodes, grant access to TSpider nodes;
- Generate *my.cnf* file, enables MGR if number of nodes >= 3;
- Write routing information to the `mysql.servers` table on Tdbctl nodes in accordance with the host inventory, and execute `tdbctl flush routing`.

At this step you can login to Tdbctl nodes to see if the routing information is right and MGR is working correctly.
```
MySQL [(none)]> select * from mysql.servers;
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
| Server_name | Host           | Db | Username     | Password     | Port  | Socket | Wrapper | Owner |
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
| SPIDER0     | 192.168.1.106  |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPIDER1     | 192.168.1.26   |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPIDER2     | 192.168.1.2    |    | tendbcluster | tendbcluster | 25000 |        | SPIDER  |       |
| SPT0        | 192.168.1.95   |    | tendbcluster | tendbcluster | 20000 |        | mysql   |       |
| SPT1        | 192.168.1.95   |    | tendbcluster | tendbcluster | 20001 |        | mysql   |       |
| SPT2        | 192.168.1.2    |    | tendbcluster | tendbcluster | 20002 |        | mysql   |       |
| SPT3        | 192.168.1.2    |    | tendbcluster | tendbcluster | 20003 |        | mysql   |       |
| TDBCTL0     | 192.168.1.26   |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
| TDBCTL1     | 192.168.1.2    |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
| TDBCTL2     | 192.168.1.106  |    | tendbcluster | tendbcluster | 26000 |        | TDBCTL  |       |
+-------------+----------------+----+--------------+--------------+-------+--------+---------+-------+
10 rows in set (0.00 sec)

MySQL [(none)]> select * from performance_schema.replication_group_members;
+---------------------------+--------------------------------------+----------------+-------------+--------------+
| CHANNEL_NAME              | MEMBER_ID                            | MEMBER_HOST    | MEMBER_PORT | MEMBER_STATE |
+---------------------------+--------------------------------------+----------------+-------------+--------------+
| group_replication_applier | 4b3fecb6-8547-11ea-8533-1405094fa102 | 192.168.1.2    |       26000 | ONLINE       |
| group_replication_applier | 4b7e4a68-8547-11ea-901a-14050942d51a | 192.168.1.26   |       26000 | ONLINE       |
| group_replication_applier | 4b80af29-8547-11ea-b347-140564753c6a | 192.168.1.106  |       26000 | ONLINE       |
+---------------------------+--------------------------------------+----------------+-------------+--------------+
3 rows in set (0.00 sec)
```

### 2.5 Deploy Monitor (Currently Unavailable)

```
ansible-playbook -i hosts.tendbcluster deploy_monitor.yml
```

Login to any TSpider node and create a database and table to verify.

## 3. One-Step Deployment

We serialize all steps above in the *deploy_tendbcluster.yml* playbook so the deployment can be done in one step.

```
ansible-playbook -i hosts.tendbcluster deploy_tendbcluster.yml

# if you set mysql password encrypted
ansible-playbook -i hosts.tendbcluster deploy_tendbcluster.yml --vault-id --vault-id password_file
```