USE sql_store;

## IF(expression, first, second) # expression 为 true时，返回first，否则返回second

SELECT
	order_id,
    order_date,
    IF(YEAR(order_date) = YEAR(NOW()), "active", "archived") AS category
FROM orders;


-- exercise

SELECT 
	product_id,
	name,
	COUNT(*) AS orders,
    IF(COUNT(*) > 1, "Many times", "Once") AS frequency
FROM products
JOIN order_items
	USING (product_id)
GROUP BY product_id, name   # group by 的时候一定要把select里的列都用上
