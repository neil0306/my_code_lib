SELECT * 
FROM customers
-- WHERE points >= 1000 AND points <=3000  -- 冗余写法
WHERE points BETWEEN 1000 AND 3000;   -- BETWEEN AND 描述了一个区间，注意这个区间是 “左闭右闭” 区间！


-- exercise:
SELECT *
FROM customers
WHERE birth_date BETWEEN '1990-01-01' AND '2000-01-01';