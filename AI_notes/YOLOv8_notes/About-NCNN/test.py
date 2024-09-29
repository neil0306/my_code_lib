from ultralytics import YOLO

# Load the YOLOv8 model
# model = YOLO("yolov8n.pt")
model = YOLO("yolov10n.pt")

# Export the model to NCNN format
model.export(format="ncnn") # creates '/yolov8n_ncnn_model'


# model.export(format="onnx") # test for yolov10


# Load the exported NCNN model
# ncnn_model = YOLO("./yolov8n_ncnn_model")
ncnn_model = YOLO("./yolov10n_ncnn_model", task='detect')

# Run inference
# results = ncnn_model("https://ultralytics.com/images/bus.jpg")
results = ncnn_model("./bus.jpg")
