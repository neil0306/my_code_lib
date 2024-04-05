SELECT *
FROM customers
-- LIMIT 3    -- 限制返回数据的数量
LIMIT 6, 3; -- 第一个数字6被称为 offset, 表示跳过前面6行数据；后面的3表示限制返回多个条数据，这里限制为3条FOR
			-- 这在某些场景很有用，比如网页中显示 第6-8行 的数据等等。

			-- LIMIT 语句一定是放在SQL语句的【最后】！

-- exercise
SELECT *
FROM customers
-- WHERE
ORDER BY points DESC  -- 按照积分进行降序排列
LIMIT 3