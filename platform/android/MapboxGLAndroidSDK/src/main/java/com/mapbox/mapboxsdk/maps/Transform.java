package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.util.concurrent.TimeUnit;

import static com.mapbox.mapboxsdk.maps.MapView.REGION_DID_CHANGE_ANIMATED;

/**
 * Resembles the current Map transformation.
 * <p>
 * Responsible for synchronising {@link CameraPosition} state and notifying {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraChangeListener}.
 * </p>
 */
class Transform {

    private NativeMapView mapView;
    private MapboxMap mapboxMap;
    private MyLocationView myLocationView;
    private float screenDensity;
    private CameraPosition cameraPosition;
    private MapboxMap.CancelableCallback cameraCancelableCallback;
    private MapboxMap.OnCameraChangeListener onCameraChangeListener;

    Transform(NativeMapView mapView, MapboxMap mapboxMap, MyLocationView myLocationView) {
        this.mapView = mapView;
        this.mapboxMap = mapboxMap;
        this.screenDensity = mapView.getPixelRatio();
        this.myLocationView = myLocationView;
    }

    //
    // Camera API
    //

    @UiThread
    public final CameraPosition getCameraPosition() {
        if (cameraPosition == null) {
            cameraPosition = invalidateCameraPosition();
        }
        return cameraPosition;
    }

    @UiThread
    void updateCameraPosition(@NonNull CameraPosition position) {
        if (myLocationView != null) {
            myLocationView.setCameraPosition(position);
        }
        mapboxMap.getMarkerViewManager().setTilt((float) position.tilt);
    }

    @UiThread
    final void moveCamera(CameraUpdate update, MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);
        cancelTransitions();
        mapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);

        // MapChange.REGION_DID_CHANGE_ANIMATED is not called for `jumpTo`
        // invalidate camera position to provide OnCameraChange event.
        mapboxMap.invalidateCameraPosition();
        if (callback != null) {
            callback.onFinish();
        }
    }

    @UiThread
    final void easeCamera(CameraUpdate update, int durationMs, boolean easingInterpolator, boolean resetTrackingMode, final MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        if (resetTrackingMode) {
            mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);
        }

        cancelTransitions();
        if (callback != null) {
            cameraCancelableCallback = callback;
            mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapView.MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cameraCancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        mapView.removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mapView.easeTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom, easingInterpolator);
    }

    @UiThread
    final void animateCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);

        cancelTransitions();
        if (callback != null) {
            cameraCancelableCallback = callback;
            mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapView.MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cameraCancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        mapView.removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mapView.flyTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom);
    }

    @UiThread
    @Nullable
    CameraPosition invalidateCameraPosition() {
        CameraPosition cameraPosition = null;
        if (mapView != null) {
            cameraPosition = new CameraPosition.Builder(mapView.getCameraValues()).build();
            this.cameraPosition = cameraPosition;
            if (onCameraChangeListener != null) {
                onCameraChangeListener.onCameraChange(this.cameraPosition);
            }
        }
        return cameraPosition;
    }

    void cancelTransitions() {
        if (cameraCancelableCallback != null) {
            cameraCancelableCallback.onCancel();
            cameraCancelableCallback = null;
        }
        mapView.cancelTransitions();
    }

    @UiThread
    void resetNorth() {
        cancelTransitions();
        mapView.resetNorth();
    }

    void setOnCameraChangeListener(@Nullable MapboxMap.OnCameraChangeListener listener) {
        this.onCameraChangeListener = listener;
    }

    private long getDurationNano(long durationMs) {
        return durationMs > 0 ? TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS) : 0;
    }

    //
    // non Camera API
    //

    // Zoom in or out

    double getZoom() {
        return cameraPosition.zoom;
    }

    void zoom(boolean zoomIn) {
        zoom(zoomIn, -1.0f, -1.0f);
    }

    void zoom(boolean zoomIn, float x, float y) {
        // Cancel any animation
        mapboxMap.cancelTransitions();

        if (zoomIn) {
            mapView.scaleBy(2.0, x / screenDensity, y / screenDensity, MapboxConstants.ANIMATION_DURATION);
        } else {
            mapView.scaleBy(0.5, x / screenDensity, y / screenDensity, MapboxConstants.ANIMATION_DURATION);
        }
    }

    // Direction
    double getBearing() {
        double direction = -mapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    void setBearing(double bearing) {
        if (myLocationView != null) {
            myLocationView.setBearing(bearing);
        }
        mapView.setBearing(bearing);
    }

    void setBearing(double bearing, float focalX, float focalY) {
        if (myLocationView != null) {
            myLocationView.setBearing(bearing);
        }
        mapView.setBearing(bearing, focalX, focalY);
    }


    //
    // LatLng / CenterCoordinate
    //

    LatLng getLatLng() {
        return mapView.getLatLng();
    }

    //
    // Pitch / Tilt
    //

    double getTilt() {
        return mapView.getPitch();
    }

    void setTilt(Double pitch) {
        if (myLocationView != null) {
            myLocationView.setTilt(pitch);
        }
        mapboxMap.getMarkerViewManager().setTilt(pitch.floatValue());
        mapView.setPitch(pitch, 0);
    }

    //
    // Center coordinate
    //

    LatLng getCenterCoordinate() {
        return mapView.getLatLng();
    }

    void setCenterCoordinate(LatLng centerCoordinate) {
        mapView.setLatLng(centerCoordinate);
    }

}
