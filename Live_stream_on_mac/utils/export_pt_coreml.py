import sys
import subprocess
import os

# Install and import packages
def install_and_import(package):
    try:
        __import__(package)
    except ImportError:
        print(f"Package '{package}' not found. Installing...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", package])
        __import__(package)

install_and_import("ultralytics")
install_and_import("coremltools")


from ultralytics import YOLO
import coremltools as ct


def export_coreml(model_name="yolo11m"):
    # check if the model pt file exists
    pt_path = f"model_weights/{model_name}.pt"
    if not os.path.exists(pt_path):
        print(f"Model weight file does not exist: {pt_path}")
        print("Downloading model weights from https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11m.pt ...")
        os.makedirs(os.path.dirname(pt_path), exist_ok=True)
        import urllib.request
        url = "https://github.com/ultralytics/assets/releases/download/v8.3.0/yolo11m.pt"
        try:
            urllib.request.urlretrieve(url, pt_path)
            print(f"Downloaded model weights to {pt_path}")
        except Exception as e:
            print(f"Failed to download model weights: {e}")
            return
    model = YOLO(f"model_weights/{model_name}.pt")
    model.export(
        format="coreml",
        imgsz=640,
        batch=1,                # coreml only support batch=1
        verbose=True,
        half=True,
        device="mps",
    )


def check_coreml(model_name="yolo11m"):
    model = ct.models.MLModel(f"model_weights/{model_name}.mlpackage")
    print(model.input_description)
    print(model.output_description)



if __name__ == "__main__":
    # model_name = "yolo11m"
    model_name = "YOLO11M_100_re"
    export_coreml(model_name=model_name)
    check_coreml(model_name=model_name)