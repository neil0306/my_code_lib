
-- 介绍SQL查询语句的基本构成

USE sql_store;  -- 选中某个小数据库（一个数据库中有多个表格），数据库管理器MySQL可以管理多个数据库。

SELECT * 
FROM customers  -- 指定拿数据的表格名称
WHERE customer_id = 1   -- 按照指定条件过滤 select 到的结果
ORDER BY first_name; -- 按照指定条件对结果进行排序

-- SQL的最基本的语句块就是上面的4个部分
-- 在SQL中，空格，换行符都会被忽略
-- SQL语句不区分大小写，但是大家约定 keyword 都用全大写字母来区分
-- 在写完一个SQL查询语句后，记得加上英文分号 ; 否则书写下一个SQL语句块的时候会报错。