from flask import Flask, request, jsonify
import numpy as np
import wave
import os
import time
import threading
import assemblyai as aai

app = Flask(__name__)

global_audio_samples = np.array([], dtype=np.int16)
last_received_time = time.time()
result = {"status": "error", "message": "No transcription available"}

aai.settings.api_key = "your API key"

config = aai.TranscriptionConfig(speech_model=aai.SpeechModel.nano, language_code="id")

def transcribe_audio():
    global result
    try:
        transcriber = aai.Transcriber(config=config)
        transcript = transcriber.transcribe('./audio2.wav')

        if transcript.status == aai.TranscriptStatus.error:
            result = {"status": "error", "message": transcript.error}
        else:
            result = {"status": "success", "transcript": transcript.text}
    except Exception as e:
        result = {"status": "error", "message": str(e)}

def check_and_transcribe():
    global last_received_time
    while True:
        time.sleep(4)
        current_time = time.time()
        if current_time - last_received_time >= 4:
            transcribe_audio()

@app.route('/upload', methods=['POST'])
def upload_audio():
    global global_audio_samples, last_received_time

    try:
        current_time = time.time()
        if current_time - last_received_time > 5:
            if os.path.exists('audio2.wav'):
                os.remove('audio2.wav')
            global_audio_samples = np.array([], dtype=np.int16)

        last_received_time = current_time

        if 'audioData' in request.form:
            audio_data = request.form['audioData']
            samples = list(map(int, audio_data.split(',')))

            new_audio_samples = np.array(samples, dtype=np.int16)

            global_audio_samples = np.concatenate((global_audio_samples, new_audio_samples))

            with wave.open('audio2.wav', 'wb') as wf:
                wf.setnchannels(1)
                wf.setsampwidth(2)
                wf.setframerate(16000)
                wf.writeframes(global_audio_samples.tobytes())

            return jsonify({"status": "success"})
        else:
            return jsonify({"status": "error", "message": "No audioData field in form"})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)})

@app.route('/transcription', methods=['GET'])
def get_transcription():
    print(result)
    return jsonify(result)

if __name__ == '__main__':
    threading.Thread(target=check_and_transcribe, daemon=True).start()
    app.run(host='0.0.0.0', port=5555)
