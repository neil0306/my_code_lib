# redis-cli
- [ ] 列出所有 keys  
    - keys *

- [ ] 列出某个 key 下的所有 field 
    - hgetall key 名


# 在 Python 程序中检查 redis 中的某个 key 数据的过期时间
```
# TTL 返回值含义：
import redis

redis_conn = redis.Redis(unix_socket_path=None, db=0, charset="utf-8")  # 连接到redis数据库

ttl = redis_conn.ttl('key_name')

if ttl == -1:
    print("键永不过期")
elif ttl == -2:
    print("键不存在")
else:
    print(f"键将在 {ttl} 秒后过期")
``` 














