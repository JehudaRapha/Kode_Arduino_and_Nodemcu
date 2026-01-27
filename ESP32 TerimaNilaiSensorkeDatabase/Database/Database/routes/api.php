<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\SensorController;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');

Route::get('/sensors/today', [SensorController::class, 'totalToday']);
Route::get('/sensors/inside', [SensorController::class, 'inside']); // route baru
Route::apiResource('sensors', SensorController::class);

