SELECT *
FROM orders
INNER JOIN customers 
	ON orders.customer_id = customers.customer_id;
    
SELECT order_id, O.customer_id, first_name, last_name  -- customer_id 在两个表格都存在，所以需要指定一下是哪个表格
FROM orders AS O			-- 起个别名，简化语句
INNER JOIN customers AS C   -- 起个别名，简化语句
	ON O.customer_id = C.customer_id;

-- SQL 里一共就两种join： inner 或者 outer，inner join 的inner可以省略不写
-- 语法结构： inner join xxxxx ON xxx
				-- 此时运行的效果是：两个表格的数据都会存在，然后按照 ON 所指定的列数据进行进一步处理：
					-- 把两个表格中指定的列数据值相同的行拼接到一起，注意此时会存在两列ON所指定的数据



-- exercise：将 order_items 和 products 用INNER JOIN 连接，然后显示的数据只包含 order_id, product_id, quantity 和 unit_price
SELECT order_id, OI.product_id, quantity, OI.unit_price
FROM order_items AS OI
JOIN products AS P
	ON OI.product_id = P.product_id;
 
