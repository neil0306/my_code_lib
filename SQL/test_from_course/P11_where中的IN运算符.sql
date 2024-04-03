SELECT *
FROM Customers
-- WHERE state = 'VA' OR state = 'GA' OR state = 'FL'  -- 查询相同的列，但是需要检查多个值的场景
-- WHERE state IN ('VA', 'FL', 'GA')   -- 用IN运算符简化表达式，括号里的字符串顺序并不会影响结果。
WHERE state NOT IN ('VA', 'FL', 'GA'); -- 还可以结合 NOT 进行排除

-- exercise
SELECT *
FROM products
WHERE quantity_in_stock IN (49, 38, 72)