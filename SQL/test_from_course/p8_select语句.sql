SELECT 
	first_name, 
    last_name, 
    points, 
    (points + 10) * 100 AS 'discount factor' -- 在选择列的时候还可以加上表达式，调整执行顺序最好加上括号
    -- 书写顺序会执行影响显示结果
FROM customers; 

-- select 语句中选择的列的顺序会直接影响到显示结果
-- select 语句中可以直接对某一列使用数学表达式
-- 使用 AS 关键字可以修改某一列的显示名称（注意原来的表格并不会被修改），新名称可以用 单引号 或者 双引号 括起来，方便编辑器高亮；不括起来也行


SELECT DISTINCT state   -- DISTINCT 关键词可以去重 
FROM customers;


-- exercise：
SELECT 
	name,
    unit_price,
    unit_price * 1.1 as 'new price'
FROM products;
    

