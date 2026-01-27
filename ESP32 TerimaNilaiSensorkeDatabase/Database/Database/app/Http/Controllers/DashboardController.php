<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\Models\Sensor;

class DashboardController extends Controller
{
    public function index()
    {
        // Hitung total masuk & keluar
        $totalMasuk = Sensor::where('sensor_name', 'masuk')->count();
        $totalKeluar = Sensor::where('sensor_name', 'keluar')->count();

        // Hitung total orang di dalam
        $totalOrang = $totalMasuk - $totalKeluar;

        // Ambil data "masuk" terbaru sebanyak sisa orang di dalam
        $dataMasuk = Sensor::where('sensor_name', 'masuk')
            ->orderBy('created_at', 'desc')
            ->take($totalOrang)
            ->get()
            ->reverse() // biar urutannya dari yang lama ke terbaru
            ->values();

        // Tambahkan label "Orang ke-"
        $dataMasuk->transform(function ($item, $key) {
            $item->orang_ke = 'Orang ke-' . ($key + 1);
            return $item;
        });

        return view('index', compact('dataMasuk', 'totalOrang'));
    }
}
