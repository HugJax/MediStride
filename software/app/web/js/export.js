// MediStride — Export CSV des sessions

export function buildCsv(samples, patientId, sessionStartIso) {
    const headers = [
        'patient_id', 'session_start', 'side', 'timestamp_ms_device', 'received_at_iso',
        'fsr1_raw', 'fsr2_raw', 'fsr3_raw', 'fsr4_raw', 'fsr5_raw', 'fsr6_raw',
        'fsr1_n', 'fsr2_n', 'fsr3_n', 'fsr4_n', 'fsr5_n', 'fsr6_n',
        'total_force_n'
    ];

    const lines = [headers.join(',')];
    for (const s of samples) {
        const row = [
            csvEscape(patientId),
            sessionStartIso,
            s.side,
            s.timestampMs,
            s.receivedAt,
            ...s.raw,
            ...s.forces.map(f => f.toFixed(2)),
            s.totalForce.toFixed(2)
        ];
        lines.push(row.join(','));
    }
    return lines.join('\n');
}

function csvEscape(value) {
    if (value == null) return '';
    const str = String(value);
    if (/[",\n]/.test(str)) {
        return '"' + str.replace(/"/g, '""') + '"';
    }
    return str;
}

export function downloadCsv(csv, filename) {
    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}
