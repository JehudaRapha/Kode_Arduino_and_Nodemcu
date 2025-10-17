<?php

namespace App\Http\Controllers;

use App\Models\Sensor;
use Illuminate\Http\Request;
use App\Http\Resources\SensorResource;

class SensorController extends Controller
{
    // GET /api/sensors
    public function index()
    {
        $sensor = Sensor::latest()->first(); // record terakhir
        return new SensorResource($sensor);
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
