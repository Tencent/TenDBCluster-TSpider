if (!`SELECT count(*) FROM information_schema.engines WHERE
      (support = 'YES' OR support = 'DEFAULT') AND
      engine = 'aria'`){
  skip Need Aria engine;
}

--source include/set_optimizer_switch.inc
