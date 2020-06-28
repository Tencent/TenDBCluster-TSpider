-- add by will; move from spider storage, install spider table when mysql_install_db
CREATE TABLE IF NOT EXISTS `mysql`.`spider_link_failed_log` (
  `db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` char(199) COLLATE utf8_bin NOT NULL DEFAULT '',
  `link_id` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `failed_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_link_mon_servers` (
  `db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` char(199) COLLATE utf8_bin NOT NULL DEFAULT '',
  `link_id` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `sid` int(10) unsigned NOT NULL DEFAULT 0,
  `server` char(64) COLLATE utf8_bin DEFAULT NULL,
  `scheme` char(64) COLLATE utf8_bin DEFAULT NULL,
  `host` char(255) COLLATE utf8_bin DEFAULT NULL,
  `port` char(5) COLLATE utf8_bin DEFAULT NULL,
  `socket` text COLLATE utf8_bin DEFAULT NULL,
  `username` char(64) COLLATE utf8_bin DEFAULT NULL,
  `password` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_ca` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_capath` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cert` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cipher` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_key` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_verify_server_cert` tinyint(4) NOT NULL DEFAULT 0,
  `default_file` text COLLATE utf8_bin DEFAULT NULL,
  `default_group` char(64) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`db_name`,`table_name`,`link_id`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_table_crd` (
  `db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` char(199) COLLATE utf8_bin NOT NULL DEFAULT '',
  `tgt_db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `tgt_table_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `key_seq` int(10) unsigned NOT NULL DEFAULT 0,
  `cardinality` bigint(20) NOT NULL DEFAULT 0,
  PRIMARY KEY (`db_name`,`table_name`,`key_seq`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_table_position_for_recovery` (
  `db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` char(199) COLLATE utf8_bin NOT NULL DEFAULT '',
  `failed_link_id` int(11) NOT NULL DEFAULT 0,
  `source_link_id` int(11) NOT NULL DEFAULT 0,
  `file` text COLLATE utf8_bin DEFAULT NULL,
  `position` text COLLATE utf8_bin DEFAULT NULL,
  `gtid` text COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`db_name`,`table_name`,`failed_link_id`,`source_link_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

 CREATE TABLE IF NOT EXISTS `mysql`.`spider_tables` (
  `db_name` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `table_name` char(199) COLLATE utf8_bin NOT NULL DEFAULT '',
  `link_id` int(11) NOT NULL DEFAULT 0,
  `priority` bigint(20) NOT NULL DEFAULT 0,
  `server` char(64) COLLATE utf8_bin DEFAULT NULL,
  `scheme` char(64) COLLATE utf8_bin DEFAULT NULL,
  `host` char(255) COLLATE utf8_bin DEFAULT NULL,
  `port` char(5) COLLATE utf8_bin DEFAULT NULL,
  `socket` text COLLATE utf8_bin DEFAULT NULL,
  `username` char(64) COLLATE utf8_bin DEFAULT NULL,
  `password` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_ca` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_capath` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cert` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cipher` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_key` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_verify_server_cert` tinyint(4) NOT NULL DEFAULT 0,
  `monitoring_binlog_pos_at_failing` tinyint(4) NOT NULL DEFAULT 0,
  `default_file` text COLLATE utf8_bin DEFAULT NULL,
  `default_group` char(64) COLLATE utf8_bin DEFAULT NULL,
  `tgt_db_name` char(64) COLLATE utf8_bin DEFAULT NULL,
  `tgt_table_name` char(64) COLLATE utf8_bin DEFAULT NULL,
  `link_status` tinyint(4) NOT NULL DEFAULT 1,
  `block_status` tinyint(4) NOT NULL DEFAULT 0,
  `static_link_id` char(64) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`db_name`,`table_name`,`link_id`),
  UNIQUE KEY `uidx1` (`db_name`,`table_name`,`static_link_id`),
  KEY `idx1` (`priority`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_xa` (
  `format_id` int(11) NOT NULL DEFAULT 0,
  `gtrid_length` int(11) NOT NULL DEFAULT 0,
  `bqual_length` int(11) NOT NULL DEFAULT 0,
  `data` binary(128) NOT NULL DEFAULT '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `status` char(8) COLLATE utf8_bin NOT NULL DEFAULT '',
  PRIMARY KEY (`data`,`format_id`,`gtrid_length`),
  KEY `idx1` (`status`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_xa_failed_log` (
  `format_id` int(11) NOT NULL DEFAULT 0,
  `gtrid_length` int(11) NOT NULL DEFAULT 0,
  `bqual_length` int(11) NOT NULL DEFAULT 0,
  `data` binary(128) NOT NULL DEFAULT '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `scheme` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `host` char(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `port` char(5) COLLATE utf8_bin NOT NULL DEFAULT '',
  `socket` text COLLATE utf8_bin NOT NULL,
  `username` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `password` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `ssl_ca` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_capath` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cert` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cipher` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_key` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_verify_server_cert` tinyint(4) NOT NULL DEFAULT 0,
  `default_file` text COLLATE utf8_bin DEFAULT NULL,
  `default_group` char(64) COLLATE utf8_bin DEFAULT NULL,
  `thread_id` int(11) DEFAULT NULL,
  `status` char(8) COLLATE utf8_bin NOT NULL DEFAULT '',
  `failed_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY `idx1` (`data`,`format_id`,`gtrid_length`,`host`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_xa_member` (
  `format_id` int(11) NOT NULL DEFAULT 0,
  `gtrid_length` int(11) NOT NULL DEFAULT 0,
  `bqual_length` int(11) NOT NULL DEFAULT 0,
  `data` binary(128) NOT NULL DEFAULT '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0',
  `scheme` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `host` char(255) COLLATE utf8_bin NOT NULL DEFAULT '',
  `port` char(5) COLLATE utf8_bin NOT NULL DEFAULT '',
  `socket` text COLLATE utf8_bin NOT NULL,
  `username` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `password` char(64) COLLATE utf8_bin NOT NULL DEFAULT '',
  `ssl_ca` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_capath` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cert` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_cipher` char(64) COLLATE utf8_bin DEFAULT NULL,
  `ssl_key` text COLLATE utf8_bin DEFAULT NULL,
  `ssl_verify_server_cert` tinyint(4) NOT NULL DEFAULT 0,
  `default_file` text COLLATE utf8_bin DEFAULT NULL,
  `default_group` char(64) COLLATE utf8_bin DEFAULT NULL,
  KEY `idx1` (`data`,`format_id`,`gtrid_length`,`host`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE IF NOT EXISTS `mysql`.`spider_table_status` (
  `db_name` char(64) NOT NULL DEFAULT '',
  `table_name` char(250) NOT NULL DEFAULT '',
  `tgt_db_name` char(64) NOT NULL DEFAULT '',
  `tgt_table_name` char(64) NOT NULL DEFAULT '',
  `data_file_length` bigint(21) unsigned DEFAULT 0,
  `max_data_file_length` bigint(21) unsigned DEFAULT 0,
  `index_file_length` bigint(21) unsigned DEFAULT 0,
  `records` bigint(21) unsigned DEFAULT 0,
  `mean_rec_length` bigint(21) unsigned DEFAULT 0,
  `check_time` datetime DEFAULT NULL,
  `update_time` datetime DEFAULT NULL,
  `create_time` datetime DEFAULT NULL,
  `modify_time` datetime DEFAULT NULL,
  PRIMARY KEY (`db_name`,`table_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
