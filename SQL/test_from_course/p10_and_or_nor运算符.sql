SELECT *
FROM customers
-- WHERE birth_date > '1990-01-01' AND points > 1000  -- AND 运算符
-- WHERE birth_date > '1990-01-01' OR points > 1000  -- OR 运算符
-- WHERE birth_date > '1990-01-01' OR points > 1000 AND state = 'VA'  -- 混合运算，AND优先级比OR高
WHERE NOT (birth_date > '1990-01-01' OR points > 1000); -- 把NOT放括号里的话，等价于 WHERE birth_date <= '1990-01-01' AND points <= 1000

-- 运算顺序： AND 高于 OR；有括号先计算括号

-- exercise:  
SELECT *
FROM order_items
WHERE order_id = 6 AND (quantity * unit_price > 30)    -- 在where里也能用算数表达式