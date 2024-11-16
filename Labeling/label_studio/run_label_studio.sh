export LABEL_STUDIO_LOCAL_FILES_SERVING_ENABLED=true                            # 允许挂载本地数据集
# export LABEL_STUDIO_LOCAL_FILES_DOCUMENT_ROOT=/Users/nullptr/Downloads/data     # 修改为本地, 待标注的数据集的路径的"最顶层目录" !!!!!!! 
                                                                                # 这个路径会影响到挂载本地数据集!

export LABEL_STUDIO_LOCAL_FILES_DOCUMENT_ROOT=/Users/nullptr/Downloads/project-5-at-2024-10-31-09-41-410eb59e
nohup label-studio > ./nohup.out 2>&1 &