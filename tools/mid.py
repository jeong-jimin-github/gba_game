from mido import MidiFile

# MIDI note 번호 → GBA freq[] 인덱스 변환
# C3(48) ~ B8(107) → 0~71
def midi_note_to_index(note):
    return note - 48   # 48 = C3

def convert_midi(filename, tick_unit=120):
    mid = MidiFile(filename)
    notes = []
    
    for track in mid.tracks:
        current_time = 0
        active_notes = {}

        for msg in track:
            current_time += msg.time

            if msg.type == "note_on" and msg.velocity > 0:
                active_notes[msg.note] = current_time

            elif (msg.type == "note_off") or (msg.type == "note_on" and msg.velocity == 0):
                if msg.note in active_notes:
                    start = active_notes[msg.note]
                    length = current_time - start
                    index = midi_note_to_index(msg.note)

                    if 0 <= index <= 71:
                        notes.append((index, length))
                    del active_notes[msg.note]

    # 길이 단위를 GBA 코드 규격에 맞게 조정
    song_freq = []
    song_length = []
    for freq_index, length in notes:
        song_freq.append(freq_index)
        song_length.append(max(1, length // tick_unit))

    return song_freq, song_length


# 변환 테스트
freq, length = convert_midi("tools/untitled.mid")

print("song_freq =", freq)
print("song_length =", length)

print("notelen " + str(len(length)))