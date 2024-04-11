-- 将表格中的NULL在显示的时候替换为'未分配'
USE sql_store;

## IFNULL
SELECT 
	order_id,
    IFNULL(shipper_id, "Not assigned") AS shipper  -- null 在显示时改成了 Not assigned
FROM orders;

## COALESE
SELECT 
	order_id,
    COALESCE (shipper_id, comments, "Not assigned") AS shipper  -- 如果不是null，显示comments里的值，如果是null，替换成 Not assigned
FROM orders;




-- exercise
SELECT 
	CONCAT(first_name, " ", last_name) AS customer,
    IFNULL(phone, 'Unknown')  AS phone  -- 用 COALESEC(phone, "Uknown") 也可以
FROM customers

