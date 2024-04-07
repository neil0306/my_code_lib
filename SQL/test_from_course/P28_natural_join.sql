USE sql_store;

SELECT 
	o.order_id,
    c.first_name
FROM orders o
NATURAL JOIN customers c   -- natural join 会让SQL引擎自己找出【相同的列名】然后转换成 xx=xx 这样的语句

-- Natural JOIN 其实很危险（有时候不知道SQL引擎选的东西是不是我们想要的），不推荐用！
