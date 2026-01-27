<?php

namespace App\Http\Controllers;

use App\Models\Sensor;
use Illuminate\Http\Request;
use App\Http\Resources\SensorResource;
use Illuminate\Support\Collection;
use Carbon\Carbon;

class SensorController extends Controller
{

    public function totalToday()
    {
        // Ambil data sensor hanya untuk hari ini
        $sensors = Sensor::whereDate('created_at', Carbon::today())->get();

        // Hitung total orang masuk hari ini
        $totalMasuk = $sensors->where('sensor_name', 'masuk')->sum('value');

        // Kembalikan JSON ringkas
        return response()->json([
            'tanggal' => Carbon::today()->toDateString(),
            'total_orang_hari_ini' => $totalMasuk,
        ]);
    }

public function inside()
{
    // Ambil semua data sensor berdasarkan waktu masuk/keluar
    $sensors = Sensor::orderBy('created_at', 'asc')->get();

    $inside = new Collection(); // simpan orang yang masih di dalam

    foreach ($sensors as $sensor) {
        if ($sensor->sensor_name === 'masuk') {
            $inside->push($sensor);
        } elseif ($sensor->sensor_name === 'keluar' && $inside->count() > 0) {
            $inside->shift(); // hapus orang pertama (FIFO)
        }
    }

    return response()->json([
        'total_orang' => $inside->count(), // jumlah orang di dalam
        'data' => $inside                     // list orang yang masih di dalam
    ]);
}

public function index()
{
    // Ambil data sensor hanya untuk hari ini
    $sensors = Sensor::whereDate('created_at', Carbon::today())
                     ->orderBy('created_at', 'asc')
                     ->get();

    // Hitung total masuk dan keluar
    $totalMasuk  = $sensors->where('sensor_name', 'masuk')->sum('value');
    $totalKeluar = $sensors->where('sensor_name', 'keluar')->sum('value');

    // Hitung jumlah orang yang masih di dalam
    $orangDiDalam = $totalMasuk - $totalKeluar;

    // Kembalikan data lengkap + hasil perhitungan
    return response()->json([
        'data' => SensorResource::collection($sensors),
        'summary' => [
            'total_masuk'     => $totalMasuk,
            'total_keluar'    => $totalKeluar,
            'orang_di_dalam'  => $orangDiDalam,
            'tanggal'         => Carbon::today()->toDateString(),
        ]
    ]);
}


    // POST /api/sensors
    public function store(Request $request)
    {
        $request->validate([
            'sensor_name' => 'required|string',
            'value'       => 'required|numeric',
        ]);

        $sensor = Sensor::create($request->only('sensor_name','value'));

        return new SensorResource($sensor);
    }

    // GET /api/sensors/{id}
    public function show(Sensor $sensor)
    {
        $sensor = Sensor::latest()->first(); // record terakhir
        return new SensorResource($sensor);
    }

    // PUT/PATCH /api/sensors/{id}
    public function update(Request $request, Sensor $sensor)
    {
        $request->validate([
            'sensor_name' => 'sometimes|string',
            'value'       => 'sometimes|numeric',
        ]);

        $sensor->update($request->only('sensor_name','value'));

        return new SensorResource($sensor);
    }

    // DELETE /api/sensors/{id}
    public function destroy(Sensor $sensor)
    {
        $sensor->delete();
        return response()->json(['status' => 'deleted']);
    }
}
