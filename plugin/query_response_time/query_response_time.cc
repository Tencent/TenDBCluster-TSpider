#include "mysql_version.h"
#include "my_global.h"
#ifdef HAVE_RESPONSE_TIME_DISTRIBUTION
#include "mysql_com.h"
#include "rpl_tblmap.h"
#include "table.h"
#include "field.h"
#include "sql_show.h"
#include "query_response_time.h"

#define TIME_STRING_POSITIVE_POWER_LENGTH QRT_TIME_STRING_POSITIVE_POWER_LENGTH
#define TIME_STRING_NEGATIVE_POWER_LENGTH 6
#define TOTAL_STRING_POSITIVE_POWER_LENGTH QRT_TOTAL_STRING_POSITIVE_POWER_LENGTH
#define TOTAL_STRING_NEGATIVE_POWER_LENGTH 6
#define MINIMUM_BASE 2
#define MAXIMUM_BASE QRT_MAXIMUM_BASE
#define POSITIVE_POWER_FILLER QRT_POSITIVE_POWER_FILLER
#define NEGATIVE_POWER_FILLER QRT_NEGATIVE_POWER_FILLER
#define TIME_OVERFLOW   QRT_TIME_OVERFLOW
#define DEFAULT_BASE    QRT_DEFAULT_BASE

#define do_xstr(s) do_str(s)
#define do_str(s) #s
#define do_format(filler,width) "%" filler width "lld"
/*
  Format strings for snprintf. Generate from:
  POSITIVE_POWER_FILLER and TIME_STRING_POSITIVE_POWER_LENGTH
  NEFATIVE_POWER_FILLER and TIME_STRING_NEGATIVE_POWER_LENGTH
*/
#define TIME_STRING_POSITIVE_POWER_FORMAT do_format(POSITIVE_POWER_FILLER,do_xstr(TIME_STRING_POSITIVE_POWER_LENGTH))
#define TIME_STRING_NEGATIVE_POWER_FORMAT do_format(NEGATIVE_POWER_FILLER,do_xstr(TIME_STRING_NEGATIVE_POWER_LENGTH))
#define TIME_STRING_FORMAT		      TIME_STRING_POSITIVE_POWER_FORMAT "." TIME_STRING_NEGATIVE_POWER_FORMAT

#define TOTAL_STRING_POSITIVE_POWER_FORMAT do_format(POSITIVE_POWER_FILLER,do_xstr(TOTAL_STRING_POSITIVE_POWER_LENGTH))
#define TOTAL_STRING_NEGATIVE_POWER_FORMAT do_format(NEGATIVE_POWER_FILLER,do_xstr(TOTAL_STRING_NEGATIVE_POWER_LENGTH))
#define TOTAL_STRING_FORMAT		      TOTAL_STRING_POSITIVE_POWER_FORMAT "." TOTAL_STRING_NEGATIVE_POWER_FORMAT

#define TIME_STRING_LENGTH	QRT_TIME_STRING_LENGTH
#define TIME_STRING_BUFFER_LENGTH	(TIME_STRING_LENGTH + 1 /* '\0' */)

#define TOTAL_STRING_LENGTH	QRT_TOTAL_STRING_LENGTH
#define TOTAL_STRING_BUFFER_LENGTH	(TOTAL_STRING_LENGTH + 1 /* '\0' */)

/*
  Calculate length of "log linear"
  1)
  (MINIMUM_BASE ^ result) <= (10 ^ STRING_POWER_LENGTH) < (MINIMUM_BASE ^ (result + 1))

  2)
  (MINIMUM_BASE ^ result) <= (10 ^ STRING_POWER_LENGTH)
  and
  (MINIMUM_BASE ^ (result + 1)) > (10 ^ STRING_POWER_LENGTH)

  3)
  result     <= LOG(MINIMUM_BASE, 10 ^ STRING_POWER_LENGTH)= STRING_POWER_LENGTH * LOG(MINIMUM_BASE,10)
  result + 1 >  LOG(MINIMUM_BASE, 10 ^ STRING_POWER_LENGTH)= STRING_POWER_LENGTH * LOG(MINIMUM_BASE,10)

  4) STRING_POWER_LENGTH * LOG(MINIMUM_BASE,10) - 1 < result <= STRING_POWER_LENGTH * LOG(MINIMUM_BASE,10)

  MINIMUM_BASE= 2 always, LOG(MINIMUM_BASE,10)= 3.3219280948873626, result= (int)3.3219280948873626 * STRING_POWER_LENGTH

  Last counter always use for time overflow
*/
#define POSITIVE_POWER_COUNT ((int)(3.32192809 * TIME_STRING_POSITIVE_POWER_LENGTH))
#define NEGATIVE_POWER_COUNT ((int)(3.32192809 * TIME_STRING_NEGATIVE_POWER_LENGTH))
#define OVERALL_POWER_COUNT (NEGATIVE_POWER_COUNT + 1 + POSITIVE_POWER_COUNT)

#define START_POS_TIME 50000 /* 50ms */

#define MILLION ((unsigned long)1000 * 1000)

namespace query_response_time
{

class utility
{
public:
  utility() : m_base(0)
  {
    m_max_dec_value= MILLION;
    for(int i= 0; TIME_STRING_POSITIVE_POWER_LENGTH > i && i < 5; ++i)
      m_max_dec_value *= 10;
    setup(DEFAULT_BASE);
  }
public:
  uint      base()            const { return m_base; }
  uint      negative_count()  const { return m_negative_count; }
  uint      start_pos()       const { return m_start_pos; }
  uint      positive_count()  const { return m_positive_count; }
  uint      bound_count()     const { return m_bound_count; }
  time_t    start_time()      const { return m_start_time; }
  ulonglong max_dec_value()   const { return m_max_dec_value; }
  ulonglong bound(uint index) const { return m_bound[ index ]; }
public:
    void setup(uint base)
    {
        m_start_time = my_time(MY_WME);
        if (base != m_base)
        {
            m_base = base;

            const ulonglong million = 1000 * 1000;
            ulonglong value = million;
            m_negative_count = 0;
            m_start_pos = 0;
            while (value > 10)
            {
                m_negative_count += 1;
                value /= m_base;
            }
            m_negative_count -= 1;

            value = million;
            m_positive_count = 0;
            while (value < m_max_dec_value)
            {
                m_positive_count += 1;
                value *= m_base;
            }
            m_bound_count = m_negative_count + m_positive_count;

            value = million;
            for (uint i = 0; i < m_negative_count; ++i)
            {
                value /= m_base;
                m_bound[m_negative_count - i - 1] = value;
                if (value > START_POS_TIME)
                    m_start_pos = m_negative_count - i - 1;
            }
            value = million;
            for (uint i = 0; i < m_positive_count; ++i)
            {
                m_bound[m_negative_count + i] = value;
                if (m_start_pos == 0 && value > START_POS_TIME)
                    m_start_pos = m_negative_count + i;
                value *= m_base;
            }
        }
    }
private:
  uint      m_base;
  uint      m_start_pos;
  uint      m_negative_count;
  uint      m_positive_count;
  uint      m_bound_count;
  time_t    m_start_time;
  ulonglong m_max_dec_value; /* for TIME_STRING_POSITIVE_POWER_LENGTH=7 is 10000000 */
  ulonglong m_bound[OVERALL_POWER_COUNT];
};

static
void print_time(char* buffer, std::size_t buffer_size, const char* format,
                uint64 value)
{
  ulonglong second=      (value / MILLION);
  ulonglong microsecond= (value % MILLION);
  my_snprintf(buffer, buffer_size, format, second, microsecond);
}

static
void print_timestamp(time_t tm, char* buf, std::size_t buf_size)
{
    char    tmp[50];
    my_get_time_str(tm, tmp, sizeof(tmp));
    snprintf(buf, buf_size, "Since %s", tmp);
}

class time_collector
{
public:
  time_collector(utility& u) : m_utility(&u)
  { }
  ~time_collector()
  { }
  uint32 count(uint index)
  {
    return my_atomic_load32((int32*)&m_count[index]);
  }
  uint64 total(uint index)
  {
    return my_atomic_load64((int64*)&m_total[index]);
  }
  uint32 p_count(uint index)
  {
      return my_atomic_load32((int32*)&m_pcount[index]);
  }
  uint64 p_total(uint index)
  {
      return my_atomic_load64((int64*)&m_ptotal[index]);
  }
public:
  void flush()
  {
    memset((void*)&m_count,  0,sizeof(m_count));
    memset((void*)&m_total,  0,sizeof(m_total));
    memset((void*)&m_pcount, 0,sizeof(m_pcount));
    memset((void*)&m_ptotal, 0,sizeof(m_ptotal));
  }
  void collect(unsigned long sql_use_partition_count, uint64 time)
  {
    int i= 0;
    int start = m_utility->start_pos();
    if (m_utility->bound(start) < time)
        i = start + 1;
    else if (m_utility->bound(start / 2) < time)
        i = start / 2 + 1;
    for(int count= m_utility->bound_count(); count > i; ++i)
    {
      if(m_utility->bound(i) > time)
      {
        my_atomic_add32((int32*)(&m_count[i]), 1);
        my_atomic_add64((int64*)(&m_total[i]), time);

        if (sql_use_partition_count >= 2)
        {
            my_atomic_add32((int32*)(&m_pcount[i]), 1);
            my_atomic_add64((int64*)(&m_ptotal[i]), time);
        }
        break;
      }
    }
  }
private:
  utility* m_utility;
  uint32   m_count[OVERALL_POWER_COUNT + 1];
  uint64   m_total[OVERALL_POWER_COUNT + 1];
  uint32   m_pcount[OVERALL_POWER_COUNT + 1];
  uint64   m_ptotal[OVERALL_POWER_COUNT + 1];
};

class collector
{
public:
  collector() : m_time(m_utility)
  {
    m_utility.setup(DEFAULT_BASE);
    m_time.flush();
  }
public:
  void flush()
  {
    m_utility.setup(opt_query_response_time_range_base);
    m_time.flush();
  }
  int fill(THD* thd, TABLE_LIST *tables, COND *cond)
  {
    DBUG_ENTER("fill_schema_query_response_time");
    TABLE        *table= static_cast<TABLE*>(tables->table);
    Field        **fields= table->field;
    for(uint i= 0, count= bound_count() + 1 /* with overflow */; count > i; ++i)
    {
      char time[TIME_STRING_BUFFER_LENGTH];
      char total[100];
      char ptotal[100];
      if(i == bound_count())
      {
        assert(sizeof(TIME_OVERFLOW) <= TIME_STRING_BUFFER_LENGTH);
        assert(sizeof(TIME_OVERFLOW) <= TOTAL_STRING_BUFFER_LENGTH);
        memcpy(time,TIME_OVERFLOW,sizeof(TIME_OVERFLOW));
        //memcpy(total,TIME_OVERFLOW,sizeof(TIME_OVERFLOW));
        //memcpy(ptotal,TIME_OVERFLOW,sizeof(TIME_OVERFLOW));
        print_timestamp(m_utility.start_time(), total, sizeof(total));
        print_timestamp(m_utility.start_time(), ptotal, sizeof(ptotal));
      }
      else
      {
        print_time(time, sizeof(time), TIME_STRING_FORMAT, this->bound(i));
        print_time(total, sizeof(total), TOTAL_STRING_FORMAT, this->total(i));
        print_time(ptotal, sizeof(ptotal), TOTAL_STRING_FORMAT, this->ptotal(i));
      }
      fields[0]->store(time,strlen(time),system_charset_info);
      fields[1]->store((longlong)this->count(i),true);
      fields[2]->store(total,strlen(total),system_charset_info);
      fields[3]->store((longlong)this->pcount(i),true);
      fields[4]->store(ptotal, strlen(ptotal), system_charset_info);
      if (schema_table_store_record(thd, table))
      {
	DBUG_RETURN(1);
      }
    }
    DBUG_RETURN(0);
  }
  void collect(unsigned long sql_use_partition_count, ulonglong time)
  {
    m_time.collect(sql_use_partition_count, time);
  }
  uint bound_count() const
  {
    return m_utility.bound_count();
  }
  ulonglong bound(uint index)
  {
    return m_utility.bound(index);
  }
  ulonglong count(uint index)
  {
    return m_time.count(index);
  }
  ulonglong total(uint index)
  {
    return m_time.total(index);
  }
  ulonglong pcount(uint index)
  {
      return m_time.p_count(index);
  }
  ulonglong ptotal(uint index)
  {
      return m_time.p_total(index);
  }
private:
  utility          m_utility;
  time_collector   m_time;
};

static collector g_collector;

} // namespace query_response_time

void query_response_time_init()
{
}

void query_response_time_free()
{
  query_response_time::g_collector.flush();
}

int query_response_time_flush()
{
  query_response_time::g_collector.flush();
  return 0;
}
void query_response_time_collect(unsigned long sql_use_partition_count, ulonglong query_time)
{
  query_response_time::g_collector.collect(sql_use_partition_count, query_time);
}

int query_response_time_fill(MYSQL_THD thd, TABLE_LIST *tables, COND *cond)
{
  return query_response_time::g_collector.fill(thd, tables, cond);
}
#endif // HAVE_RESPONSE_TIME_DISTRIBUTION
