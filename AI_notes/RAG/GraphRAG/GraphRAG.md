
# 论文笔记





---

# 本地部署 (Mac)
这是按照微软官方教程走的。

## 安装
创建虚拟环境 & 激活虚拟环境
```shell
conda create -n graphrag python=3.10 
conda activate graphrag
```

安装 graphrag 
```shell
pip install graphrag
```

找一个地方作为知识库 (这里直接在当前路径下新建一个文件夹)
```shell
# 新建文件夹以存放数据
mkdir -p ./ragtest/input 

# 将原始数据放入文件夹中 (这里按照官方教程下载一份测试数据, 一共3970行内容的书)
curl https://www.gutenberg.org/cache/epub/24022/pg24022.txt > ./ragtest/input/book.txt
```

初始化环境变量 & 构建文档索引信息
```shell
python -m graphrag.index --init --root ./ragtest
```
- `python -m`: “-m”选项允许将库模块作为脚本运行。它在“sys.path”中搜索模块，并将其作为脚本运行。
- `graphrag.index --init --root ./ragtest`: `--init`初始化环境变量，创建`.env`和`settings.yaml`文件; 然后将`settings.yaml`中的`root`字段设置为`./ragtest`.
    > `.env` contains the environment variables required to run the GraphRAG pipeline. 
    >> If you inspect the file, you'll see a single environment variable defined, GRAPHRAG_API_KEY=<API_KEY>. This is the API key for the OpenAI API or Azure OpenAI endpoint. You can replace this with your own API key.
    >
    > `settings.yaml` contains the settings for the pipeline. 
    >> You can modify this file to change the settings for the pipeline.
    - `settings.yaml`中 LLM 有关的重要配置：
        ```yaml
        type: azure_openai_chat # Or azure_openai_embedding for embeddings
        api_base: https://<instance>.openai.azure.com
        api_version: 2024-02-15-preview # You can customize this for other versions
        deployment_name: <azure_model_deployment_name>
        ```
        - 更多详细信息去[官方文档](https://microsoft.github.io/graphrag/posts/get_started)查看。

- 运行后，还会在`./ragtest`文件夹下生成两个文件夹：`output`和`prompts`
  - `output`文件夹下主要按日期和时间生成 `reports`和`artifacts`, report 主要存放的是 `indexing-engine.log` 和 `logs.json` 等日志，`artifacts` 则是一些 `.parquet` 文件。
  - `prompts`文件夹下存放了一些 prompt 模板文件，这些模板会按照`settings.yaml`中的配置来调用。


修改好`.env`和`settings.yaml`文件后，就可以正式构建知识库索引了
```shell
python -m graphrag.index --root ./ragtest
```
- 输出如下：
![](GraphRAG_images/构建indexing的输出.png)

- 仅构建这本书的内容，OpenAI 收费大概 2.5 美金，比较贵！
  ![](GraphRAG_images/GraphRAG构建index费用.png)



---

## 关于 settings.yaml
```yaml
encoding_model: cl100k_base
skip_workflows: []
llm:
  api_key: ${GRAPHRAG_API_KEY}     # 这个 key 在.env 文件中进行设置
  type: openai_chat # or azure_openai_chat
  model: gpt-4-turbo-preview     # 指定模型，想省点钱的话可以使用 gpt-4o
  model_supports_json: true # recommended if this is available for your model.

  # ---- 设置输入模型的最大 token 数 ----
  # max_tokens: 4000      

  # ---- 设置请求超时时间 ----
  # request_timeout: 180.0

  # ---- 如果需要改用本地模型或者其他模型 (如 groq 提供的 llama)，则需要修改下面的参数 -----
  # api_base: https://<instance>.openai.azure.com
  # api_version: 2024-02-15-preview


  # organization: <organization_id>
  # deployment_name: <azure_model_deployment_name>
  # tokens_per_minute: 150_000 # set a leaky bucket throttle
  # requests_per_minute: 10_000 # set a leaky bucket throttle
  # max_retries: 10
  # max_retry_wait: 10.0
  # sleep_on_rate_limit_recommendation: true # whether to sleep when azure suggests wait-times
  # concurrent_requests: 25 # the number of parallel inflight requests that may be made

parallelization:
  stagger: 0.3
  # num_threads: 50 # the number of threads to use for parallel processing

async_mode: threaded # or asyncio

embeddings:
  ## parallelization: override the global parallelization settings for embeddings
  async_mode: threaded # or asyncio
  llm:
    api_key: ${GRAPHRAG_API_KEY}
    type: openai_embedding # or azure_openai_embedding
    model: text-embedding-3-small
    # api_base: https://<instance>.openai.azure.com
    # api_version: 2024-02-15-preview
    # organization: <organization_id>
    # deployment_name: <azure_model_deployment_name>
    # tokens_per_minute: 150_000 # set a leaky bucket throttle
    # requests_per_minute: 10_000 # set a leaky bucket throttle
    # max_retries: 10
    # max_retry_wait: 10.0
    # sleep_on_rate_limit_recommendation: true # whether to sleep when azure suggests wait-times
    # concurrent_requests: 25 # the number of parallel inflight requests that may be made
    # batch_size: 16 # the number of documents to send in a single request
    # batch_max_tokens: 8191 # the maximum number of tokens to send in a single request
    # target: required # or optional
  


chunks:
  size: 300       # 每一份 chunk 包含的最大 token 数
  overlap: 100    # 每一份 chunk 的重叠部分 (如果没有重叠的部分，模型很容易丢失上下文)
  group_by_columns: [id] # by default, we don't allow chunks to cross documents
    
input:
  type: file # or blob
  file_type: text # or csv
  base_dir: "input"
  file_encoding: utf-8
  file_pattern: ".*\\.txt$"

cache:
  type: file # or blob
  base_dir: "cache"
  # connection_string: <azure_blob_storage_connection_string>
  # container_name: <azure_blob_storage_container_name>

storage:
  type: file # or blob
  base_dir: "output/${timestamp}/artifacts"
  # connection_string: <azure_blob_storage_connection_string>
  # container_name: <azure_blob_storage_container_name>

reporting:
  type: file # or console, blob
  base_dir: "output/${timestamp}/reports"
  # connection_string: <azure_blob_storage_connection_string>
  # container_name: <azure_blob_storage_container_name>

entity_extraction:
  ## llm: override the global llm settings for this task
  ## parallelization: override the global parallelization settings for this task
  ## async_mode: override the global async_mode settings for this task
  prompt: "prompts/entity_extraction.txt"      # 用于识别 entity 的 prompt 模板 (entity 指的是 organization, person, geo, event 等，在下面的 entity_types 中指定)
  entity_types: [organization,person,geo,event]
  max_gleanings: 0

summarize_descriptions:
  ## llm: override the global llm settings for this task
  ## parallelization: override the global parallelization settings for this task
  ## async_mode: override the global async_mode settings for this task
  prompt: "prompts/summarize_descriptions.txt"
  max_length: 500

claim_extraction:
  ## llm: override the global llm settings for this task
  ## parallelization: override the global parallelization settings for this task
  ## async_mode: override the global async_mode settings for this task
  # enabled: true
  prompt: "prompts/claim_extraction.txt"
  description: "Any claims or facts that could be relevant to information discovery."
  max_gleanings: 0

community_report:
  ## llm: override the global llm settings for this task
  ## parallelization: override the global parallelization settings for this task
  ## async_mode: override the global async_mode settings for this task
  prompt: "prompts/community_report.txt"
  max_length: 2000
  max_input_length: 8000

cluster_graph:
  max_cluster_size: 10

embed_graph:
  enabled: false # if true, will generate node2vec embeddings for nodes
  # num_walks: 10
  # walk_length: 40
  # window_size: 2
  # iterations: 3
  # random_seed: 597832

umap:
  enabled: false # if true, will generate UMAP embeddings for nodes

snapshots:
  graphml: false
  raw_entities: false
  top_level_nodes: false

local_search:
  # text_unit_prop: 0.5
  # community_prop: 0.1
  # conversation_history_max_turns: 5
  # top_k_mapped_entities: 10
  # top_k_relationships: 10
  # max_tokens: 12000

global_search:
  # max_tokens: 12000
  # data_max_tokens: 12000
  # map_max_tokens: 1000
  # reduce_max_tokens: 2000
  # concurrency: 32
```




--- 

# Reference 
- 微软官方介绍：https://microsoft.github.io/graphrag/

- 原论文：https://arxiv.org/pdf/2404.16130
