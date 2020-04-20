# Transaction Scenarios

TenDB cluster supports both local transactions and distributed transactions. This document provides user's guides and best practices for TenDB cluster.


## User's Guide
See [Transactions](../re-book/transaction-en.md/#usage)

## Guidelines
Below parameters should be considered when using transactions:
1. Parameters for distributed transactions: [SPIDER_INTERNAL_XA](../re-book/transaction-en.md/#usage)
2. Parameter should be enabled when initializing a distributed transaction: [spider_trans_rollback](../re-book/tspider-parameter-en.md/#spider_trans_rollback) to ensure transaction Atomicity.
3. To avoid the autocommit=0 option in the application framework, which may lead to unexpected behavior, parameter  [SPIDER_IGNORE_AUTOCOMMIT](../re-book/tspider-parameter-en.md/#SPIDER_IGNORE_AUTOCOMMIT) can be enabled.
4. To avoid performance overhead, disable [SPIDER_WITH_BEGIN_COMMIT](../re-book/tspider-parameter-en.md/#SPIDER_WITH_BEGIN_COMMIT). Performance benefits can be gained by not issuing begin/commit to remote TSpider for non-explicit transactions.

## Suspended Transactions Handling
See [Distributed transactions](../re-book/mysql-compatibility-en.md#jump253)


## Transactional Switchover
See [SQL reference](../re-book/new-grammar-en.md#write_lock)

Issue below commands:
```
update mysql.servers;
flush table with write lock;
flush privileges;
unlock tables;
```