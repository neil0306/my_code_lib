import os
import json
from tqdm import tqdm

def convert_labelimg_to_labelstudio(input_dir, output_file):
    result = []
    image_files = [f for f in os.listdir(input_dir) if f.endswith('.jpg')]
    
    for image_file in tqdm(image_files, desc="Converting files"):
        image_path = os.path.join(input_dir, image_file)
        txt_file = os.path.splitext(image_file)[0] + '.txt'
        txt_path = os.path.join(input_dir, txt_file)
        
        if not os.path.exists(txt_path):
            print(f"Warning: No corresponding txt file for {image_file}")
            continue
        
        try:
            with open(txt_path, 'r') as f:
                annotations = []
                for line in f:
                    class_id, x_center, y_center, width, height = map(float, line.strip().split())
                    annotations.append({
                        "type": "rectanglelabels",
                        "value": {
                            "x": x_center - width/2,
                            "y": y_center - height/2,
                            "width": width,
                            "height": height,
                            "rotation": 0,
                            "rectanglelabels": [f"Class_{int(class_id)}"]
                        }
                    })
                
                result.append({
                    "data": {
                        "image": f"/data/local-files/?d={image_path}"
                    },
                    "annotations": [{
                        "result": annotations
                    }]
                })
        except Exception as e:
            print(f"Error processing {txt_file}: {str(e)}")
    
    with open(output_file, 'w') as f:
        json.dump(result, f, indent=2)

    print(f"Conversion complete. Output saved to {output_file}")

if __name__ == "__main__":
    # 使用示例
    input_directory = "./data/labelimg"
    output_json_file = "./data/labelstudio.json"
    convert_labelimg_to_labelstudio(input_directory, output_json_file)

