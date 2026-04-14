#!/bin/bash

# RTSP Streaming Server Script
# Converts video files to RTSP streams using FFmpeg with loop playback support
# Uses re-encoding for better compatibility and reduced warnings

# Display help information
show_help() {
    echo "Usage: $0 <video_file> [loop_option]"
    echo ""
    echo "Arguments:"
    echo "  <video_file>    Path to the video file to stream"
    echo "  [loop_option]   Optional parameters:"
    echo "    -l, --loop    Loop playback (default: no loop)"
    echo "    -h, --help    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 test.mp4              # Play test.mp4 once"
    echo "  $0 test.mp4 --loop       # Loop playback test.mp4"
    echo "  $0 /path/to/video.mp4 -l # Loop playback specified video"
    echo ""
    echo "RTSP Stream URL: rtsp://localhost:8554/test_video"
    echo ""
    echo "Notes:"
    echo "- Ensure FFmpeg is installed"
    echo "- Default port 8554, modify RTSP_URL variable in script if needed"
    echo "- Press Ctrl+C to stop streaming"
}

# Check argument count
if [ $# -eq 0 ]; then
    echo "Error: Missing video file argument"
    echo ""
    show_help
    exit 1
fi

# Default parameters
VIDEO_FILE=""
LOOP_MODE=false
RTSP_URL="rtsp://localhost:8554/test_video"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -l|--loop)
            LOOP_MODE=true
            shift
            ;;
        -*)
            echo "Error: Unknown option $1"
            echo ""
            show_help
            exit 1
            ;;
        *)
            if [ -z "$VIDEO_FILE" ]; then
                VIDEO_FILE="$1"
            else
                echo "Error: Only one video file allowed"
                echo ""
                show_help
                exit 1
            fi
            shift
            ;;
    esac
done

# Check if video file exists
if [ ! -f "$VIDEO_FILE" ]; then
    echo "Error: Video file '$VIDEO_FILE' does not exist"
    exit 1
fi

# Check if FFmpeg is installed
if ! command -v ffmpeg &> /dev/null; then
    echo "Error: FFmpeg not installed. Please install FFmpeg:"
    echo "macOS: brew install ffmpeg"
    echo "Ubuntu/Debian: sudo apt install ffmpeg"
    echo "CentOS/RHEL: sudo yum install ffmpeg"
    exit 1
fi

echo "=== RTSP Streaming Server ==="
echo "Video File: $VIDEO_FILE"
echo "Loop Mode: $(if $LOOP_MODE; then echo "Yes"; else echo "No"; fi)"
echo "RTSP URL: $RTSP_URL"
echo "=============================="
echo ""

# Build FFmpeg command
FFMPEG_CMD="ffmpeg -re"

if $LOOP_MODE; then
    FFMPEG_CMD="$FFMPEG_CMD -stream_loop -1"
    echo "Loop playback mode enabled, video will repeat continuously"
else
    echo "Single play mode, will stop after playback completes"
fi

FFMPEG_CMD="$FFMPEG_CMD -i \"$VIDEO_FILE\" -c:v libx264 -c:a aac -preset ultrafast -tune zerolatency -g 30 -f rtsp \"$RTSP_URL\""

echo ""
echo "Starting RTSP streaming..."
echo "Command: $FFMPEG_CMD"
echo ""
echo "Press Ctrl+C to stop streaming"
echo ""

# Execute FFmpeg command
eval $FFMPEG_CMD

# Check exit status
if [ $? -eq 0 ]; then
    echo ""
    echo "RTSP streaming stopped"
else
    echo ""
    echo "Warning: FFmpeg exited abnormally, exit code: $?"
fi
