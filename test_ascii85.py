#!/usr/bin/env python3
import pytest
import base64
import sys
import subprocess
import tempfile
import os


def encode(data):
    """Вызывает C++ функцию encode из ascii85.cpp"""
    with tempfile.NamedTemporaryFile(delete=False) as input_file:
        input_file.write(data)
        input_path = input_file.name

    output_path = input_path + ".encoded"

    try:
        result = subprocess.run(["./ascii85", "encode", input_path, output_path],
                                capture_output=True, text=False, check=True)

        with open(output_path, "rb") as f:
            encoded_data = f.read()

        return encoded_data
    finally:
        # Удаляем временные файлы
        if os.path.exists(input_path):
            os.remove(input_path)
        if os.path.exists(output_path):
            os.remove(output_path)


def decode(data):
    """Вызывает C++ функцию decode из ascii85.cpp"""
    with tempfile.NamedTemporaryFile(delete=False) as input_file:
        input_file.write(data)
        input_path = input_file.name

    output_path = input_path + ".decoded"

    try:
        result = subprocess.run(["./ascii85", "decode", input_path, output_path],
                                capture_output=True, text=False, check=True)

        with open(output_path, "rb") as f:
            decoded_data = f.read()

        return decoded_data
    finally:
        # Удаляем временные файлы
        if os.path.exists(input_path):
            os.remove(input_path)
        if os.path.exists(output_path):
            os.remove(output_path)


def test_encode_with_base64():
    # Тестовые данные
    test_data = [
        b"Hello, World!",
        b"Python is awesome",
        b"Special chars: !@#$%^&*()",
        "Привет, мир!".encode('utf-8'),  # Unicode текст, закодированный в UTF-8
        b"",  # Пустая строка
    ]

    for data in test_data:
        # Кодируем данные с помощью нашего кодировщика
        our_encoded = encode(data)
        # Кодируем данные с помощью стандартного base64
        base64_encoded = base64.a85encode(data)

        # Проверяем, что результаты совпадают
        assert our_encoded == base64_encoded


def test_decode_with_base64():
    # Тестовые данные
    test_data = [
        b"Hello, World!",
        b"Python is awesome",
        b"Special chars: !@#$%^&*()",
        "Привет, мир!".encode('utf-8'),  # Unicode текст, закодированный в UTF-8
        b"",  # Пустая строка
    ]

    for data in test_data:
        # Кодируем данные с помощью стандартного base64
        encoded = base64.a85encode(data)
        # Декодируем данные с помощью нашего декодировщика
        our_decoded = decode(encoded)

        # Проверяем, что результаты совпадают
        assert our_decoded == data


def test_decode_invalid_data():
    # Тестовые случаи с некорректными данными
    invalid_data = [
        b"Invalid ASCII85 data!@#$",  # Некорректные символы
        b"<~",  # Неполные данные
        b"<~!@#$%^&*()~>",  # Некорректная длина
    ]

    for data in invalid_data:
        with pytest.raises(subprocess.CalledProcessError) as exc_info:
            decode(data)
        assert exc_info.value.returncode != 0  # Проверяем, что код выхода не 0


if __name__ == "__main__":
    pytest.main([__file__]) 