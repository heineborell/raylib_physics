#pragma once
inline constexpr int WIDTH{1000};
inline constexpr int HEIGHT{1000};
inline constexpr int FPS{10000};
inline constexpr int NCELLS{200};
inline double dt{1.0 / 60.0};
inline double counter{0.0};

inline constexpr double PARTICLE_RADIUS{1};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{10};
inline constexpr int BINS{50};

inline constexpr double zoomSpeed{1.1};
extern bool isRunning;
