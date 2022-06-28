/* Copyright (C) 2012-2017 Kentoku Shiba

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

#define MYSQL_SERVER 1
#include <my_global.h>
#include "mysql_version.h"
#include "spd_environ.h"
#if MYSQL_VERSION_ID < 50500
#include "mysql_priv.h"
#include <mysql/plugin.h>
#else
#include "sql_priv.h"
#include "probes_mysql.h"
#include "sql_class.h"
#include "sql_partition.h"
#include "sql_show.h"
#endif
#include "spd_db_include.h"
#include "spd_include.h"
#include "spd_table.h"
#include "spd_param.h"

extern pthread_mutex_t spider_mem_calc_mutex;

extern const char *spider_alloc_func_name[SPIDER_MEM_CALC_LIST_NUM];
extern const char *spider_alloc_file_name[SPIDER_MEM_CALC_LIST_NUM];
extern ulong spider_alloc_line_no[SPIDER_MEM_CALC_LIST_NUM];
extern ulonglong spider_total_alloc_mem[SPIDER_MEM_CALC_LIST_NUM];
extern longlong spider_current_alloc_mem[SPIDER_MEM_CALC_LIST_NUM];
extern ulonglong spider_alloc_mem_count[SPIDER_MEM_CALC_LIST_NUM];
extern ulonglong spider_free_mem_count[SPIDER_MEM_CALC_LIST_NUM];

extern HASH spider_conn_meta_info;
// extern pthread_mutex_t spider_conn_meta_mutex;
extern mysql_rwlock_t spider_conn_meta_rwlock;
extern void spider_free_conn_meta(void *);
extern int spider_param_conn_meta_max_invalid_duration();

extern Time_zone *spd_tz_system;

extern mysql_mutex_t spider_active_conns_mutex;
extern HASH spider_active_conns;

static struct st_mysql_storage_engine spider_i_s_info = {
    MYSQL_INFORMATION_SCHEMA_INTERFACE_VERSION};

static ST_FIELD_INFO spider_i_s_alloc_mem_fields_info[] = {
    {"ID", 10, MYSQL_TYPE_LONG, 0, MY_I_S_UNSIGNED, "id", SKIP_OPEN_TABLE},
    {"FUNC_NAME", 64, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "func_name",
     SKIP_OPEN_TABLE},
    {"FILE_NAME", 64, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "file_name",
     SKIP_OPEN_TABLE},
    {"LINE_NO", 10, MYSQL_TYPE_LONG, 0, MY_I_S_UNSIGNED | MY_I_S_MAYBE_NULL,
     "line_no", SKIP_OPEN_TABLE},
    {"TOTAL_ALLOC_MEM", 20, MYSQL_TYPE_LONGLONG, 0,
     MY_I_S_UNSIGNED | MY_I_S_MAYBE_NULL, "total_alloc_mem", SKIP_OPEN_TABLE},
    {"CURRENT_ALLOC_MEM", 20, MYSQL_TYPE_LONGLONG, 0, MY_I_S_MAYBE_NULL,
     "current_alloc_mem", SKIP_OPEN_TABLE},
    {"ALLOC_MEM_COUNT", 20, MYSQL_TYPE_LONGLONG, 0,
     MY_I_S_UNSIGNED | MY_I_S_MAYBE_NULL, "alloc_mem_count", SKIP_OPEN_TABLE},
    {"FREE_MEM_COUNT", 20, MYSQL_TYPE_LONGLONG, 0,
     MY_I_S_UNSIGNED | MY_I_S_MAYBE_NULL, "free_mem_count", SKIP_OPEN_TABLE},
    {NULL, 0, MYSQL_TYPE_STRING, 0, 0, NULL, 0}};

static ST_FIELD_INFO spider_i_s_conn_pool_info[] = {
    {"CONN_ID", 10, MYSQL_TYPE_LONG, 0, MY_I_S_UNSIGNED, "conn_id",
     SKIP_OPEN_TABLE},
    {"USER", 64, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "user",
     SKIP_OPEN_TABLE},
    {"IP", 64, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "ip", SKIP_OPEN_TABLE},
    {"PORT", 10, MYSQL_TYPE_LONG, 0, MY_I_S_UNSIGNED, "port", SKIP_OPEN_TABLE},
    {"ALLOC_TIME", 32, MYSQL_TYPE_DATETIME, 0, MY_I_S_MAYBE_NULL, "start_time",
     SKIP_OPEN_TABLE},
    {"LAST_VISIT_TIME", 32, MYSQL_TYPE_DATETIME, 0, MY_I_S_MAYBE_NULL,
     "last_visit_time", SKIP_OPEN_TABLE},
    {"FREE_TIME", 32, MYSQL_TYPE_DATETIME, 0, MY_I_S_MAYBE_NULL, "end_time",
     SKIP_OPEN_TABLE},
    {"REUSAGE_COUNTER", 10, MYSQL_TYPE_LONGLONG, 0, MY_I_S_UNSIGNED,
     "reusage_counter", SKIP_OPEN_TABLE},
    /* active/idle/invalid */
    {"STATUS", 16, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "status",
     SKIP_OPEN_TABLE},
    {NULL, 0, MYSQL_TYPE_STRING, 0, 0, NULL, 0}};

static ST_FIELD_INFO spider_i_s_active_conns_fields_info[] = {
    {"CONN_ID", 4, MYSQL_TYPE_LONGLONG, 0, 0, "Conn_id", SKIP_OPEN_TABLE},
    {"THD_ID", 4, MYSQL_TYPE_LONGLONG, 0, MY_I_S_MAYBE_NULL, "Thd_id",
     SKIP_OPEN_TABLE},
    {"REMOTE_THD_ID", 4, MYSQL_TYPE_LONGLONG, 0, MY_I_S_MAYBE_NULL, "Remote_thd_id",
     SKIP_OPEN_TABLE},
    {"HOST", HOSTNAME_LENGTH, MYSQL_TYPE_STRING, 0, 0, "Host", SKIP_OPEN_TABLE},
    {"PORT", 4, MYSQL_TYPE_LONG, 0, 0, "Port", SKIP_OPEN_TABLE},
    {"DB", NAME_CHAR_LEN, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "Db",
     SKIP_OPEN_TABLE},
    {"COMMAND", 16, MYSQL_TYPE_STRING, 0, 0, "Command", SKIP_OPEN_TABLE},
    {"TIME", 7, MYSQL_TYPE_LONG, 0, 0, "Time", SKIP_OPEN_TABLE},
    {"STATE", 64, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL, "State",
     SKIP_OPEN_TABLE},
    {"INFO", PROCESS_LIST_INFO_WIDTH, MYSQL_TYPE_STRING, 0, MY_I_S_MAYBE_NULL,
     "Info", SKIP_OPEN_TABLE},
};

static int spider_i_s_alloc_mem_fill_table(THD *thd, TABLE_LIST *tables,
                                           COND *cond) {
  uint roop_count;
  TABLE *table = tables->table;
  DBUG_ENTER("spider_i_s_alloc_mem_fill_table");
  for (roop_count = 0; roop_count < SPIDER_MEM_CALC_LIST_NUM; roop_count++) {
    table->field[0]->store(roop_count, TRUE);
    if (spider_alloc_func_name[roop_count]) {
      table->field[1]->set_notnull();
      table->field[2]->set_notnull();
      table->field[3]->set_notnull();
      table->field[4]->set_notnull();
      table->field[5]->set_notnull();
      table->field[6]->set_notnull();
      table->field[7]->set_notnull();
      table->field[1]->store(spider_alloc_func_name[roop_count],
                             strlen(spider_alloc_func_name[roop_count]),
                             system_charset_info);
      table->field[2]->store(spider_alloc_file_name[roop_count],
                             strlen(spider_alloc_file_name[roop_count]),
                             system_charset_info);
      table->field[3]->store(spider_alloc_line_no[roop_count], TRUE);
      pthread_mutex_lock(&spider_mem_calc_mutex);
      table->field[4]->store(spider_total_alloc_mem[roop_count], TRUE);
      table->field[5]->store(spider_current_alloc_mem[roop_count], FALSE);
      table->field[6]->store(spider_alloc_mem_count[roop_count], TRUE);
      table->field[7]->store(spider_free_mem_count[roop_count], TRUE);
      pthread_mutex_unlock(&spider_mem_calc_mutex);
    } else {
      table->field[1]->set_null();
      table->field[2]->set_null();
      table->field[3]->set_null();
      table->field[4]->set_null();
      table->field[5]->set_null();
      table->field[6]->set_null();
      table->field[7]->set_null();
    }
    if (schema_table_store_record(thd, table)) {
      DBUG_RETURN(1);
    }
  }
  DBUG_RETURN(0);
}

static void my_fill_field(void *entry, void *data1, void *data2) {
  DBUG_ENTER("my_fill_field");
  SPIDER_CONN_META_INFO *meta = (SPIDER_CONN_META_INFO *)entry;
  TABLE *table = (TABLE *)data1;
  THD *thd = (THD *)data2;
  if (meta && table) {
    table->field[0]->set_notnull();
    table->field[1]->set_notnull();
    table->field[2]->set_notnull();
    table->field[3]->set_notnull();

    table->field[0]->store(meta->conn_id, true);
    table->field[1]->store(meta->remote_user_str, strlen(meta->remote_user_str),
                           system_charset_info);
    table->field[2]->store(meta->remote_ip_str, strlen(meta->remote_ip_str),
                           system_charset_info);
    table->field[3]->store(meta->remote_port, true);
    MYSQL_TIME ts;
    if (meta->alloc_tm > 0) {
      table->field[4]->set_notnull();
      spider_make_mysql_time(&ts, &meta->alloc_tm);
      table->field[4]->store_time(&ts);
    }
    if (meta->last_visit_tm > 0) {
      table->field[5]->set_notnull();
      spider_make_mysql_time(&ts, &meta->last_visit_tm);
      table->field[5]->store_time(&ts);
    }
    if (meta->free_tm > 0) {
      table->field[6]->set_notnull();
      spider_make_mysql_time(&ts, &meta->free_tm);
      table->field[6]->store_time(&ts);
    }
    table->field[7]->set_notnull();
    table->field[7]->store(meta->reusage_counter, true);
    table->field[8]->set_notnull();
    table->field[8]->store(SPIDER_CONN_META_STATUS_TO_STR(meta),
                           strlen(SPIDER_CONN_META_STATUS_TO_STR(meta)),
                           system_charset_info);
  }

  if (schema_table_store_record(thd, table)) {
    spider_my_err_logging("[ERROR] table store record for SPIDER_CONNS!\n");
  }

  DBUG_VOID_RETURN;
}

static void my_record_and_fill_field(void *entry, void *pool, va_list args) {
  DBUG_ENTER("my_record_and_fill_field");
  SPIDER_CONN_META_INFO *meta = (SPIDER_CONN_META_INFO *)entry;
  TABLE *table = va_arg(args, TABLE *);
  THD *thd = va_arg(args, THD *);
  DYNAMIC_STRING_ARRAY *invalid_meta_hash_value_arr =
      va_arg(args, DYNAMIC_STRING_ARRAY *);
  DYNAMIC_STRING_ARRAY *invalid_meta_key_arr =
      va_arg(args, DYNAMIC_STRING_ARRAY *);
  if (SPIDER_CONN_IS_INVALID(meta)) {
#ifdef SPIDER_HAS_HASH_VALUE_TYPE
    append_dynamic_string_array(invalid_meta_hash_value_arr,
                                (char *)&meta->key_hash_value,
                                sizeof(meta->key_hash_value));
#else
    HASH *hash = (HASH *)pool;
    my_hash_value_type hash_value =
        my_calc_hash(hash, meta->key, meta->key_len);
    append_dynamic_string_array(invalid_meta_hash_value_arr,
                                (char *)&hash_value, sizeof(hash_value));
#endif
    append_dynamic_string_array(invalid_meta_key_arr, (char *)meta->key,
                                meta->key_len);
  }

  my_fill_field(meta, table, thd);
  DBUG_VOID_RETURN;
}

static int spider_i_s_conn_pool_fill_table(THD *thd, TABLE_LIST *tables,
                                           COND *cond) {
  DBUG_ENTER("spider_i_s_conn_pool_fill_table");
  DYNAMIC_STRING_ARRAY invalid_meta_hash_value_arr;
  DYNAMIC_STRING_ARRAY invalid_meta_key_arr;
  init_dynamic_string_array(&invalid_meta_hash_value_arr, 64, 64);
  init_dynamic_string_array(&invalid_meta_key_arr, 64, 64);
  mysql_rwlock_rdlock(&spider_conn_meta_rwlock);
  my_hash_delegate_nargs(&spider_conn_meta_info, my_record_and_fill_field,
                         (void *)tables->table, (void *)thd,
                         (void *)&invalid_meta_hash_value_arr,
                         (void *)&invalid_meta_key_arr);
  mysql_rwlock_unlock(&spider_conn_meta_rwlock);
  /* need delete invalid item from spider_conn_meta_info hash table */
  for (size_t i = 0; i < invalid_meta_key_arr.cur_idx; ++i) {
    my_hash_value_type *tmp_ptr = NULL;
    if (get_dynamic_string_array(&invalid_meta_hash_value_arr,
                                 (char **)&tmp_ptr, NULL, i)) {
      spider_my_err_logging(
          "[ERROR] fill tmp_ptr pointer from invalid_meta_hash_value_arr "
          "error!\n");
      break;
    }

    my_hash_value_type meta_key_hash_value = *tmp_ptr;
    char *meta_key = NULL;
    size_t meta_key_len;
    if (get_dynamic_string_array(&invalid_meta_key_arr, &meta_key,
                                 &meta_key_len, i)) {
      spider_my_err_logging(
          "[ERROR] fill meta_key pointer from invalid_meta_key_arr error!\n");
      break;
    }
    mysql_rwlock_wrlock(&spider_conn_meta_rwlock);
#ifdef SPIDER_HAS_HASH_VALUE_TYPE
    SPIDER_CONN_META_INFO *meta =
        (SPIDER_CONN_META_INFO *)my_hash_search_using_hash_value(
            &spider_conn_meta_info, meta_key_hash_value, (uchar *)meta_key,
            meta_key_len);
#else
    SPIDER_CONN_META_INFO *meta = (SPIDER_CONN_META_INFO *)my_hash_search(
        &spider_conn_meta_info, meta_key, meta_key_len);
#endif
    if (meta) {
      time_t cur_tm = time(NULL);
      if (cur_tm - meta->free_tm >=
          spider_param_conn_meta_max_invalid_duration()) {
        my_hash_delete(&spider_conn_meta_info, (uchar *)meta);
      }
    }
    mysql_rwlock_unlock(&spider_conn_meta_rwlock);
  }

  free_dynamic_string_array(&invalid_meta_hash_value_arr);
  free_dynamic_string_array(&invalid_meta_key_arr);
  DBUG_RETURN(0);
}

static const char *spider_conn_get_command_name(SPIDER_CONN *conn) {
  switch (conn->m_command) {
    case SPD_COM_SLEEP:       return "Sleep";
    case SPD_COM_FREE:        return "Free";
    case SPD_COM_DISCONNECT:  return "Disconnect";
    case SPD_COM_KILLED:      return "Killed";
    case SPD_COM_CONNECT:     return "Connect";
    case SPD_COM_QUERY:       return "Query";
    case SPD_COM_END:
    default:                  return "Unknown";
  }
}

static int spider_i_s_active_conns_fill_table(THD *thd, TABLE_LIST *tables,
                                              COND *cond) {
  DBUG_ENTER("spider_i_s_active_conns_fill_table");
  if (!spider_param_enable_active_conns_view()) DBUG_RETURN(0);

  bool got_status_lock;
  SPIDER_CONN *conn;
  const char *db, *command_name;
  bool enable_info = spider_param_active_conns_view_info_length();
  TABLE *table = tables->table;
  CHARSET_INFO *cs = system_charset_info;
  ulonglong utime, unow = microsecond_interval_timer();

  if (!thd->killed) {
    mysql_mutex_lock(&spider_active_conns_mutex);
    for (uint i = 0; i < spider_active_conns.records; i++) {
      conn = (SPIDER_CONN *)my_hash_element(&spider_active_conns, i);

      /* restore_record */
      memcpy(table->record[0], table->s->default_values,
             (size_t)table->s->reclength);
      /* Conn ID */
      table->field[0]->store((longlong)conn->conn_id, TRUE);

      for (int j = 0; j < 100; j++) {
        /*
          Use try-lock here to avoid hanging. The loop is short because status
          mutex should never be locked for a long time.
        */
        if ((got_status_lock = !mysql_mutex_trylock(&conn->m_status_mutex)))
          break;
        LF_BACKOFF();
      }
      if (got_status_lock) {
        /* Conn owner thread ID */
        if (conn->thd) {
          table->field[1]->store((longlong)conn->thd->thread_id, TRUE);
          table->field[1]->set_notnull();
        }
        if (conn->db_conn) {
          /* Thread ID of remote connection */
          table->field[2]->store((longlong)conn->db_conn->thread_id(), TRUE);
          table->field[2]->set_notnull();
          /* DB */
          if ((db = conn->db_conn->get_db())) {
            table->field[5]->store(db, strlen(db), cs);
            table->field[5]->set_notnull();
          }
        }
        /* State */
        if (conn->m_state) {
          table->field[8]->store(conn->m_state, strlen(conn->m_state), cs);
          table->field[8]->set_notnull();
        }
        /* Info */
        if (enable_info && conn->m_command == SPD_COM_QUERY &&
            conn->m_last_query.length()) {
          table->field[9]->store(conn->m_last_query.ptr(),
                                 conn->m_last_query.length(), &my_charset_bin);
          table->field[9]->set_notnull();
        }
        mysql_mutex_unlock(&conn->m_status_mutex);
      }

      /* Host */
      table->field[3]->store(conn->tgt_host, conn->tgt_host_length, cs);
      /* Port */
      table->field[4]->store((longlong)conn->tgt_port, TRUE);
      /* Command */
      command_name = spider_conn_get_command_name(conn);
      table->field[6]->store(command_name, strlen(command_name), cs);
      /* Time */
      utime = conn->m_start_utime;
      utime = (utime && utime < unow) ? (unow - utime) : 0;
      table->field[7]->store(utime / HRTIME_RESOLUTION, TRUE);

      if (schema_table_store_record(thd, table)) {
        mysql_mutex_unlock(&spider_active_conns_mutex);
        DBUG_RETURN(1);
      }
    }
    mysql_mutex_unlock(&spider_active_conns_mutex);
  }
  DBUG_RETURN(0);
}

static int spider_i_s_alloc_mem_init(void *p) {
  ST_SCHEMA_TABLE *schema = (ST_SCHEMA_TABLE *)p;
  DBUG_ENTER("spider_i_s_alloc_mem_init");
  schema->fields_info = spider_i_s_alloc_mem_fields_info;
  schema->fill_table = spider_i_s_alloc_mem_fill_table;
  schema->idx_field1 = 0;
  DBUG_RETURN(0);
}

static int spider_i_s_conn_pool_init(void *p) {
  ST_SCHEMA_TABLE *schema = (ST_SCHEMA_TABLE *)p;
  DBUG_ENTER("spider_i_s_conn_pool_init");
  schema->fields_info = spider_i_s_conn_pool_info;
  schema->fill_table = spider_i_s_conn_pool_fill_table;
  schema->idx_field1 = 0;
  DBUG_RETURN(0);
}

static int spider_i_s_active_conns_init(void *p) {
  ST_SCHEMA_TABLE *schema = (ST_SCHEMA_TABLE *)p;
  DBUG_ENTER("spider_i_s_conn_pool_init");
  schema->fields_info = spider_i_s_active_conns_fields_info;
  schema->fill_table = spider_i_s_active_conns_fill_table;
  schema->idx_field1 = 0;
  DBUG_RETURN(0);
}

static int spider_i_s_alloc_mem_deinit(void *p) {
  DBUG_ENTER("spider_i_s_alloc_mem_deinit");
  DBUG_RETURN(0);
}

static int spider_i_s_conn_pool_deinit(void *p) {
  DBUG_ENTER("spider_i_s_conn_pool_deinit");
  DBUG_RETURN(0);
}

static int spider_i_s_active_conns_deinit(void *p) {
  DBUG_ENTER("spider_i_s_active_conns_deinit");
  DBUG_RETURN(0);
}

struct st_mysql_plugin spider_i_s_alloc_mem = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_ALLOC_MEM",
    "Kentoku Shiba",
    "Spider memory allocating viewer",
    PLUGIN_LICENSE_GPL,
    spider_i_s_alloc_mem_init,
    spider_i_s_alloc_mem_deinit,
    0x0001,
    NULL,
    NULL,
    NULL,
#if MYSQL_VERSION_ID >= 50600
    0,
#endif
};

struct st_mysql_plugin spider_i_s_conns = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_CONNS",
    "willhan",
    "Spider connection pool viewer",
    PLUGIN_LICENSE_GPL,
    spider_i_s_conn_pool_init,
    spider_i_s_conn_pool_deinit,
    0x0001, /* plugin version */
    NULL,
    NULL,
    NULL,
#if MYSQL_VERSION_ID >= 50600
    0,
#endif
};

struct st_mysql_plugin spider_i_s_active_conns = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_ACTIVE_CONNS",
    "Daniel Ye",
    "Display for active backend connections' status",
    PLUGIN_LICENSE_GPL,
    spider_i_s_active_conns_init,
    spider_i_s_active_conns_deinit,
    0x0001,
    NULL,
    NULL,
    NULL,
#if MYSQL_VERSION_ID >= 50600
    0,
#endif
};

#ifdef MARIADB_BASE_VERSION
struct st_maria_plugin spider_i_s_alloc_mem_maria = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_ALLOC_MEM",
    "Kentoku Shiba",
    "Spider memory allocating viewer",
    PLUGIN_LICENSE_GPL,
    spider_i_s_alloc_mem_init,
    spider_i_s_alloc_mem_deinit,
    0x0100,
    NULL,
    NULL,
    "1.0",
    MariaDB_PLUGIN_MATURITY_STABLE};

struct st_maria_plugin spider_i_s_conns_maria = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_CONNS",
    "willhan",
    "Spider connection pool viewer",
    PLUGIN_LICENSE_GPL,
    spider_i_s_conn_pool_init,
    spider_i_s_conn_pool_deinit,
    0x0100,
    NULL,
    NULL,
    "1.0",
    MariaDB_PLUGIN_MATURITY_GAMMA,
};

struct st_maria_plugin spider_i_s_active_conns_maria = {
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &spider_i_s_info,
    "SPIDER_ACTIVE_CONNS",
    "Daniel Ye",
    "Display for active backend connections' status",
    PLUGIN_LICENSE_GPL,
    spider_i_s_active_conns_init,
    spider_i_s_active_conns_deinit,
    0x0001,
    NULL,
    NULL,
    "1.0",
    MariaDB_PLUGIN_MATURITY_EXPERIMENTAL,
};
#endif