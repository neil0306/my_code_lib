# 要测试多个expression并返回不同值时，就可以使用case，而不是if，if只能跑一个 expression
SELECT 
	order_id,
	CASE 
		WHEN YEAR(order_date) = YEAR(NOW()) THEN 'Active'       # case 1
        WHEN YEAR(order_date) = YEAR(NOW()) -1 THEN "last year" # case 2
		WHEN YEAR(order_date) = YEAR(NOW()) -2 THEN "Archived"  # case 3
		ELSE "Future" # 兜底用的默认条件
	END AS category
FROM orders;



-- exercise
USE sql_store;

SELECT 
	CONCAT(first_name, " ", last_name),
    points,
    CASE 
		WHEN points < 2000 THEN "Bronze"
        WHEN points BETWEEN 2000 AND 3000 THEN "Silver"
        WHEN points > 3000 THEN "Gold"
    END AS category
FROM customers
ORDER BY points DESC;
