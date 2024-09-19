from transformers_0919.src.transformers.models.qwen2 import Qwen2Config, Qwen2Model
import torch

def run_qwen2():
    qwen2config = Qwen2Config(vocab_size=151936,           # Vocabulary size of the Qwen2 model. Defines the number of different tokens that can be represented by the `inputs_ids` passed when calling [`Qwen2Model`]
                              hidden_size=4096//2,         # Dimension of the hidden representations.
                              intermediate_size=22016//2,  # Dimension of the MLP representations.
                              num_hidden_layers=32//2,     # Number of hidden layers in the Transformer encoder.
                              num_attention_heads=32,      # Number of attention heads for each attention layer in the Transformer encoder.
                              num_key_value_heads=32,      # This is the number of key_value heads that should be used to implement Grouped Query Attention.
                              max_position_embeddings=2048//2) # Maximum number of position embeddings to use. This is the maximum length that this model may be used to attend to.
    
    qwen2config = Qwen2Model(config=qwen2config)  # Initialize Qwen2Model with the configured Qwen2Config

    inputids = torch.randint(0, qwen2config.config.vocab_size, (4, 30))  # Generate random input IDs

    res = qwen2config(inputids)  # Pass input IDs through the model

    print(type(res))  # Print the type of the model's output

if __name__ == "__main__":
    run_qwen2()  # Call the run_qwen2 function when the script is run directly