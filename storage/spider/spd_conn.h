/* Copyright (C) 2008-2017 Kentoku Shiba

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#define SPIDER_LOCK_MODE_NO_LOCK 0
#define SPIDER_LOCK_MODE_SHARED 1
#define SPIDER_LOCK_MODE_EXCLUSIVE 2

#define SPIDER_BG_SIMPLE_NO_ACTION 0
#define SPIDER_BG_SIMPLE_CONNECT 1
#define SPIDER_BG_SIMPLE_DISCONNECT 2
#define SPIDER_BG_SIMPLE_RECORDS 3

#define SPIDER_CONN_STATE_COMMIT "Commit"
#define SPIDER_CONN_STATE_CONNECT "Connecting"
#define SPIDER_CONN_STATE_DISCONNECT "Disconnecting"
#define SPIDER_CONN_STATE_EXEC_QUERY "Executing query"
#define SPIDER_CONN_STATE_INIT "Init"
#define SPIDER_CONN_STATE_IN_CONN_POOL "In connection pool"
#define SPIDER_CONN_STATE_ROLLBACK "Rollback"
#define SPIDER_CONN_STATE_STORE_RESULT "Storing result"
#define SPIDER_CONN_STATE_STORE_RESULT_END "End store result"

#define SPIDER_CONN_RESET_STATUS(conn)  \
  do {                                  \
    spider_conn_reset_command(conn);    \
    spider_conn_reset_state(conn);      \
    spider_conn_reset_last_query(conn); \
    spider_conn_set_time(conn);         \
  } while (0)

class SPIDER_CONN_POOL {
public:
  SPIDER_CONN_POOL(){}

  bool init(my_hash_get_key get_key, uint init_cap, CHARSET_INFO *charset);
  void destroy();
  bool put_conn(SPIDER_CONN *conn);
  SPIDER_CONN *get_conn(my_hash_value_type v, uchar *conn, uint key_len);
  SPIDER_CONN *get_conn_by_key(uchar *conn, uint key_len);
  void iterate(my_hash_delegate_func iter_func, void *param);

  my_hash_value_type calc_hash(const uchar *key, size_t length);

private:
  HASH connections;       /* like unordered_map<string, queue<conn*> > */
  mysql_rwlock_t rw_lock; /* Read-Write Lock to secure the hash */
  bool conn_inited;       /* whether the hash and rwlock hash inited */
};

uchar *spider_conn_get_key(SPIDER_CONN *conn, size_t *length,
                           my_bool not_used __attribute__((unused)));

uchar *spider_conn_pool_get_key(void *record, size_t *length,
                                my_bool not_used __attribute__((unused)));

uchar *spider_ipport_conn_get_key(SPIDER_IP_PORT_CONN *ip_port, size_t *length,
                                  my_bool not_used __attribute__((unused)));

int spider_reset_conn_setted_parameter(SPIDER_CONN *conn, THD *thd);

int spider_free_conn_alloc(SPIDER_CONN *conn);

void spider_free_conn_from_trx(SPIDER_TRX *trx, SPIDER_CONN *conn, bool another,
                               bool trx_free, int *roop_count);

SPIDER_CONN *spider_create_conn(SPIDER_SHARE *share, ha_spider *spider,
                                int link_id, int base_link_id, uint conn_kind,
                                int *error_num);

SPIDER_CONN *spider_get_conn(SPIDER_SHARE *share, int link_idx, SPIDER_TRX *trx,
                             ha_spider *spider, bool another, bool thd_chg,
                             uint conn_kind, int *error_num);

int spider_free_conn(SPIDER_CONN *conn);

int spider_check_and_get_casual_read_conn(THD *thd, ha_spider *spider,
                                          int link_idx);

int spider_check_and_init_casual_read(THD *thd, ha_spider *spider,
                                      int link_idx);

void spider_conn_queue_connect(SPIDER_SHARE *share, SPIDER_CONN *conn,
                               int link_idx);

void spider_conn_queue_connect_rewrite(SPIDER_SHARE *share, SPIDER_CONN *conn,
                                       int link_idx);

void spider_conn_queue_ping(ha_spider *spider, SPIDER_CONN *conn, int link_idx);

void spider_conn_queue_ping_rewrite(ha_spider *spider, SPIDER_CONN *conn,
                                    int link_idx);

void spider_conn_queue_trx_isolation(SPIDER_CONN *conn, int trx_isolation);

void spider_conn_queue_semi_trx_isolation(SPIDER_CONN *conn, int trx_isolation);

void spider_conn_queue_autocommit(SPIDER_CONN *conn, bool autocommit);

void spider_conn_queue_sql_log_off(SPIDER_CONN *conn, bool sql_log_off);

void spider_conn_queue_time_zone(SPIDER_CONN *conn, Time_zone *time_zone);

void spider_conn_queue_start_transaction(SPIDER_CONN *conn);

void spider_conn_queue_xa_start(SPIDER_CONN *conn, XID *xid);

void spider_conn_clear_queue(SPIDER_CONN *conn);

void spider_conn_clear_queue_at_commit(SPIDER_CONN *conn);

void spider_conn_set_timeout(SPIDER_CONN *conn, uint net_read_timeout,
                             uint net_write_timeout);

void spider_conn_set_timeout_from_share(SPIDER_CONN *conn, int link_idx,
                                        THD *thd, SPIDER_SHARE *share);

void spider_conn_set_timeout_from_direct_sql(SPIDER_CONN *conn, THD *thd,
                                             SPIDER_DIRECT_SQL *direct_sql);

void spider_tree_insert(SPIDER_CONN *top, SPIDER_CONN *conn);

SPIDER_CONN *spider_tree_first(SPIDER_CONN *top);

SPIDER_CONN *spider_tree_last(SPIDER_CONN *top);

SPIDER_CONN *spider_tree_next(SPIDER_CONN *current);

uint spider_tree_num(SPIDER_CONN *top);

SPIDER_CONN *spider_tree_delete(SPIDER_CONN *conn, SPIDER_CONN *top);

int spider_set_conn_bg_param(ha_spider *spider);

int spider_set_conn_bg_param_for_dml(ha_spider *spider);

int spider_create_conn_thread(SPIDER_CONN *conn);

void spider_free_conn_thread(SPIDER_CONN *conn);

void spider_bg_conn_wait(SPIDER_CONN *conn);

void spider_bg_all_conn_wait(ha_spider *spider);

int spider_bg_all_conn_pre_next(ha_spider *spider, int link_idx);

void spider_bg_conn_break(SPIDER_CONN *conn, ha_spider *spider);

void spider_bg_all_conn_break(ha_spider *spider);

bool spider_bg_conn_get_job(SPIDER_CONN *conn);

int spider_bg_conn_search(ha_spider *spider, int link_idx, int first_link_idx,
                          bool first, bool pre_next, bool discard_result,
                          ulong sql_type = SPIDER_SQL_TYPE_SELECT_SQL);

void spider_bg_conn_simple_action(SPIDER_CONN *conn, uint simple_action,
                                  bool caller_wait, void *target, uint link_idx,
                                  int *error_num);

void *spider_bg_conn_action(void *arg);

int spider_create_sts_thread(SPIDER_SHARE *share);

void spider_free_sts_thread(SPIDER_SHARE *share);

void *spider_bg_sts_action(void *arg);

int spider_create_crd_thread(SPIDER_SHARE *share);

void spider_free_crd_thread(SPIDER_SHARE *share);

void *spider_bg_crd_action(void *arg);

int spider_create_mon_threads(SPIDER_TRX *trx, SPIDER_SHARE *share);

void spider_free_mon_threads(SPIDER_SHARE *share);

void *spider_bg_mon_action(void *arg);

#define SPIDER_DISABLE_LINK

#ifdef SPIDER_DISABLE_LINK
#define spider_conn_first_link_idx(thd, link_statuses, access_balances,    \
                                   conn_link_idx, link_count, link_status) \
  (0)
#define spider_conn_next_link_idx(thd, link_statuses, access_balances, \
                                  conn_link_idx, link_idx, link_count, \
                                  link_status)                         \
  (0)
#define spider_conn_link_idx_next(link_statuses, conn_link_idx, link_idx, \
                                  link_count, link_status)                \
  (link_idx + 1)

#else
int spider_conn_first_link_idx(THD *thd, long *link_statuses,
                               long *access_balances, uint *conn_link_idx,
                               int link_count, int link_status);

int spider_conn_next_link_idx(THD *thd, long *link_statuses,
                              long *access_balances, uint *conn_link_idx,
                              int link_idx, int link_count, int link_status);

int spider_conn_link_idx_next(long *link_statuses, uint *conn_link_idx,
                              int link_idx, int link_count, int link_status);
#endif

int spider_conn_get_link_status(long *link_statuses, uint *conn_link_idx,
                                int link_idx);

int spider_conn_lock_mode(ha_spider *spider);

bool spider_conn_check_recovery_link(SPIDER_SHARE *share);

bool spider_conn_use_handler(ha_spider *spider, int lock_mode, int link_idx);

bool spider_conn_need_open_handler(ha_spider *spider, uint idx, int link_idx);
uchar *spider_xid_get_hash_key(const uchar *ptr, size_t *length,
                               my_bool not_used __attribute__((unused)));
void spider_xid_free_hash(void *ptr);
SPIDER_IP_PORT_CONN *spider_create_ipport_conn(SPIDER_CONN *conn);
SPIDER_CONN *spider_get_conn_from_idle_connection(
    SPIDER_SHARE *share, int link_idx, char *conn_key, ha_spider *spider,
    uint conn_kind, int base_link_idx, int *error_num);
void spider_free_ipport_conn(void *info);
void spider_free_conn_meta(void *);
void spider_free_conn_recycle_thread(void);
int spider_create_conn_recycle_thread(void);

SPIDER_CONN_META_INFO *spider_create_conn_meta(SPIDER_CONN *);
my_bool spider_add_conn_meta_info(SPIDER_CONN *);
void spider_update_conn_meta_info(SPIDER_CONN *, uint);

uchar *spider_conn_meta_get_key(SPIDER_CONN_META_INFO *meta, size_t *length,
                                my_bool not_used __attribute__((unused)));

int spider_create_get_status_thread(void);
void spider_free_get_status_thread(void);
uint spider_create_sts_conn_key(char *key, char *host, ulong port, char *user,
                                char *passwd);
SPIDER_FOR_STS_CONN *spider_create_sts_conn(char *key, ulong key_len,
                                            char *conn);
uchar *spider_for_sts_conn_get_key(SPIDER_FOR_STS_CONN *sts_conn,
                                   size_t *length,
                                   my_bool not_used __attribute__((unused)));
void spider_free_for_sts_conn(void *info);

ulong spider_get_conn_thread_id(SPIDER_CONN *conn);

int spider_send_kill(SPIDER_CONN *, enum killed_state);

void spider_conn_reset_command(SPIDER_CONN *conn);
void spider_conn_set_command(SPIDER_CONN *conn, enum spider_conn_command cmd);
void spider_conn_reset_state(SPIDER_CONN *conn);
void spider_conn_set_state(SPIDER_CONN *conn, const char *state);
void spider_conn_set_time(SPIDER_CONN *conn);
void spider_conn_reset_last_query(SPIDER_CONN *conn);
void spider_conn_set_last_query(SPIDER_CONN *conn, const char *query, uint length, CHARSET_INFO *cs);
