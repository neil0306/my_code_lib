SELECT *
FROM customers
-- WHERE phone is NULL  -- 找出phone为NULL的数据
WHERE phone is NOT NULL -- 找出phone 不为 NULL 的数据
;

-- exercise:
SELECT *
FROM orders
WHERE shipped_date is NULL;