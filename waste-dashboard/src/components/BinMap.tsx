import React from "react";
import { MapContainer, TileLayer, Marker, Popup } from "react-leaflet";
import "leaflet/dist/leaflet.css";
import L from "leaflet";
import type { Bin, Alert } from "../types";

const createIcon = (status: string, hasAlert: boolean) =>
  new L.Icon({
    iconUrl: hasAlert
      ? "https://cdn-icons-png.flaticon.com/512/871/871564.png" // e.g. animated warning icon
      : status === "normal"
      ? "https://maps.google.com/mapfiles/ms/icons/green-dot.png"
      : status === "warning"
      ? "https://maps.google.com/mapfiles/ms/icons/yellow-dot.png"
      : "https://maps.google.com/mapfiles/ms/icons/red-dot.png",
    iconSize: [30, 41],
    iconAnchor: [12, 41],
  });

export const BinMap: React.FC<{ bins: Bin[]; alerts: Alert[] }> = ({
  bins,
  alerts,
}) => {
  const center: [number, number] = [39.3446, -76.583];

  return (
    <div className="col-span-full h-[400px] rounded overflow-hidden shadow">
      <h2 className="text-xl font-bold mb-2">Bin Locations</h2>
      <MapContainer center={center} zoom={17} className="h-full w-full z-0">
        <TileLayer
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
          attribution="&copy; OpenStreetMap contributors"
        />
        {bins.map((bin) => {
          const alert = alerts.find((a) => a.binId === bin.id);
          const hasAlert = !!alert;
          const icon = createIcon(bin.status, hasAlert);

          return (
            <Marker
              key={bin.id}
              position={[bin.location.lat, bin.location.lng]}
              icon={icon}
            >
              <Popup>
                <strong>{bin.location.name}</strong>
                <br />
                Fullness: {bin.fullness}%
                <br />
                Status: {bin.status}
                {hasAlert && (
                  <>
                    <br />
                    ⚠️ <strong>Alert:</strong> {alert.message}
                  </>
                )}
              </Popup>
            </Marker>
          );
        })}
      </MapContainer>
    </div>
  );
};
