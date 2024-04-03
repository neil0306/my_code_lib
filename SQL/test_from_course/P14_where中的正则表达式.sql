SELECT *
FROM customers
-- WHERE last_name LIKE "%field%"
-- WHERE last_name REGEXP "field|mac|rose"     -- 正则表达式，过滤 last_name 中，但凡包含 field 或者 mac 或者 rose 的数据 （不指定他们在字符串的位置）
-- WHERE last_name REGEXP "field$|^mac|^rose"  -- 匹配以field结尾，以mac或者rose开头的字符串
WHERE last_name REGEXP '[gim]e' -- 匹配任意包含 ge 或 ie 或 me 的字符串
;

-- ^ 表示开头，如 ^field 表示以 field 开头的字符串
-- $ 表示结尾，如 field$ 表示以 field 结尾的字符串
-- | 用来组合多个匹配项，匹配项之间的是 “或” 的关系
-- [] 表示匹配括号中的任意单个字符，这些字符会以枚举的形式与前面或后面的其他字符拼成一个完整表达式再进行匹配
	-- 如 [gim]e 表示匹配任意包含 ge 或 ie 或 me 的字符串
	-- e[fmq] 则表示 匹配包含  ef 或 em 或 eq 的字符串
    -- [a-h]e 表示 匹配任何 从ae, be, ce, ..., he 的字符串，a-h号表示“从a到h”的简写

-- exercise

SELECT * 
FROM customers
-- WHERE first_name IN ("ELKA", "AMBUR") -- first name 为 ELKA 或 AMBUR
   -- WHERE fist_name REGEXP "elka|ambur"
-- WHERE last_name REGEXP "EY$|ON$"   -- 以EY或ON结尾
-- WHERE last_name REGEXP "^MY|SE"  -- 以MY开头或者包含SE
WHERE last_name REGEXP "B[RU]" -- 包含b，并且b后面是 R或者U



