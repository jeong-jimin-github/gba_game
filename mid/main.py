import music21 as m21
import os

def midi_to_data(input, output):
    path = input + '.mid'
    midi = m21.converter.parse(path)
    pitchs = []
    durations = []

    for measure in midi[1]:
        for item in measure:
            if isinstance(item, m21.note.Note):
                print(int(item.pitch.frequency), int(item.quarterLength * 1000))
                pitchs.append(int(item.pitch.frequency))
                durations.append(int(item.quarterLength * 1000))
    file = open(output + '.c', 'w')
    file.write('#include "lib/gba.h"\n')
    file.write('#include "music.h"\n\n')
    file.write('static u16 ' + name + '_song_freq[] = {')
    for p in pitchs:
        file.write(str(p) + ', ')
    file.write('0};\n\n')
    file.write('static u16 ' + name + '_song_dur[] = {')
    for d in durations:
        file.write(str(d) + ', ')
    file.write('0};\n\n')
    file.write('SONG ' + name + '_song_1' + ' = {' + name + '_song_freq, ' + str(len(pitchs)) + '};\n')
    file.write('SONG ' + name + '_song_2' + ' = {' + name + '_song_dur, ' + str(len(durations)) + '};\n')
    file.close()

if __name__ == '__main__':
    # search all .mid files in the directory
    for file in os.listdir('./tools/mid/'):
        if file.endswith('.mid'):
            name = file[:-4]
            midi_to_data(name, './mid/' + name)