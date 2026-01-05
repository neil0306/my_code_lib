import torch
from jaxtyping import Float
from einops import reduce

def einops_reduce():
    # You can reduce a single tensor via some operation (e.g., sum, mean, max, min).
    x: Float[torch.Tensor, "batch seq hidden"] = torch.ones(2, 3, 4)  # @inspect x
    
    # Old way:
    y = x.mean(dim=-1)  # @inspect y
    
    # New (einops) way:
    y = reduce(x, "... hidden -> ...", "mean")  # @inspect y