import speech_recognition as sr
from gtts import gTTS
import pygame
import time
import os

# =====================
# FUNGSI SUARA (TTS)
# =====================
def bicara(teks):
    tts = gTTS(text=teks, lang='id')
    tts.save("suara.mp3")

    pygame.mixer.init()
    pygame.mixer.music.load("suara.mp3")
    pygame.mixer.music.play()

    while pygame.mixer.music.get_busy():
        time.sleep(0.1)

    pygame.mixer.quit()
    os.remove("suara.mp3")

# =====================
# FUNGSI DENGAR SUARA
# =====================
def dengar():
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("🎤 Mendengarkan...")
        r.adjust_for_ambient_noise(source, duration=0.5)
        audio = r.listen(source)

    try:
        teks = r.recognize_google(audio, language="id-ID")
        print("👤 Kamu:", teks)
        return teks.lower()
    except:
        return ""

# =====================
# PROGRAM UTAMA
# =====================
bicara(
    "Halo, selamat datang ya. "
    "Aku adalah sistem pendaftaran pintar. "
    "Kalau kamu ingin mendaftarkan wajah kamu ke sistem, "
    "silakan ucapkan dengan jelas, daftarkan muka saya."
)

perintah = dengar()

if "daftarkan muka saya" in perintah:
    bicara(
        "Baik, perintah sudah diterima. "
        "Sebelum kita lanjut ke proses pendaftaran wajah, "
        "aku mau kenal kamu dulu. "
        "Boleh tahu, siapa nama kamu?"
    )

    nama = dengar()

    if nama:
        bicara(
            f"Oke, terima kasih. "
            f"Nama anda: {nama}. "
            "Data nama sudah berhasil dicatat. "
            "Silakan bersiap untuk proses selanjutnya."
        )
        print(f"✅ Nama anda: {nama}")
    else:
        bicara(
            "Maaf ya, tadi aku belum bisa menangkap nama kamu dengan jelas. "
            "Silakan ulangi sekali lagi nanti."
        )
else:
    bicara(
        "Hmm, aku belum mendengar perintah yang sesuai. "
        "Kalau ingin mendaftar, ucapkan daftarkan muka saya ya."
    )
