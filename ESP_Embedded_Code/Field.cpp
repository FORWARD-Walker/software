#include <vector>
#include <array>
#include <cmath>
#include <iostream> // Optional: For debugging

// Define constants for the pixel map dimensions
const int PIXEL_MAP_WIDTH = 640; // Example width
const int PIXEL_MAP_HEIGHT = 480; // Example height

class Field {
private:
    const double PRINCIPAL_POINT_X = 320.0; // Example value
    const double PRINCIPAL_POINT_Y = 240.0; // Example value
    const double FOCAL_LENGTH = 500.0;      // Example value
    const double k_repulsion = 500.0;       // Repulsion scaling factor
    const double forward_bias = 0.5;        // Forward driving priority
    const double max_power = 255.0;         // Maximum motor power

    // 2D pixel map field for tracking obstacles
    std::vector<std::vector<double>> pixel_map;

public:
    // Constructor to initialize the pixel map
    Field() : pixel_map(PIXEL_MAP_HEIGHT, std::vector<double>(PIXEL_MAP_WIDTH, 0.0)) {}

    // Convert pixel coordinates to LOS vector
    std::array<double, 3> pixel2los(double i, double j) {
        double camXnorm = (i - PRINCIPAL_POINT_X) / FOCAL_LENGTH;
        double camYnorm = (j - PRINCIPAL_POINT_Y) / FOCAL_LENGTH;
        std::array<double, 3> rLOScam = {camXnorm, camYnorm, 1.0};

        double magnitude = std::sqrt(
            rLOScam[0] * rLOScam[0] +
            rLOScam[1] * rLOScam[1] +
            rLOScam[2] * rLOScam[2]
        );

        return {
            rLOScam[0] / magnitude,
            rLOScam[1] / magnitude,
            rLOScam[2] / magnitude
        };
    }

    // Update pixel map with obstacle positions and maintain memory
    void updatePixelMap(const std::vector<std::array<int, 2>>& obstacle_positions,
                        const std::vector<std::array<double, 2>>& obstacle_velocities,
                        double delta_time) {
        // Update existing obstacles based on velocity
        for (size_t i = 0; i < obstacle_positions.size(); ++i) {
            int x = obstacle_positions[i][0];
            int y = obstacle_positions[i][1];
            double vx = obstacle_velocities[i][0]; // Velocity in x direction
            double vy = obstacle_velocities[i][1]; // Velocity in y direction

            // Project obstacle position based on velocity and time
            int new_x = static_cast<int>(x + vx * delta_time);
            int new_y = static_cast<int>(y + vy * delta_time);

            // Ensure indices are within bounds
            if (new_x >= 0 && new_x < PIXEL_MAP_WIDTH && new_y >= 0 && new_y < PIXEL_MAP_HEIGHT) {
                pixel_map[new_y][new_x] = 1.0; // Mark obstacle presence
            }
        }
    }

    // Calculate the direction vector
    void calculateDirectionVector(const std::vector<std::array<double, 3>>& obstacles_los,
                                  double platform_velocity_x,
                                  double platform_velocity_y,
                                  double imu_heading,
                                  double& direction_x,
                                  double& direction_y) {
        direction_x = platform_velocity_x + forward_bias;
        direction_y = platform_velocity_y;

        for (const auto& los_vector : obstacles_los) {
            double repulsion_force = k_repulsion / (los_vector[2] + 1e-6); // Z-axis in LOS vector as distance
            direction_x -= repulsion_force * los_vector[0]; // X-component
            direction_y -= repulsion_force * los_vector[1]; // Y-component
        }

        // Normalize the direction vector
        double magnitude = std::sqrt(direction_x * direction_x + direction_y * direction_y);
        direction_x /= magnitude;
        direction_y /= magnitude;

        // Adjust based on IMU heading
        double desired_heading = atan2(direction_y, direction_x);
        double heading_correction = imu_heading - desired_heading;
        direction_x = cos(heading_correction) * direction_x - sin(heading_correction) * direction_y;
        direction_y = sin(heading_correction) * direction_x + cos(heading_correction) * direction_y;
    }

    // Map direction to motor power for differential drive
    void calculateMotorPower(double direction_x, double direction_y,
                             double& left_motor_power, double& right_motor_power) {
        double forward_component = direction_y;
        double turn_component = direction_x;

        left_motor_power = max_power * (forward_component - turn_component);
        right_motor_power = max_power * (forward_component + turn_component);

        // Normalize motor power to ensure bounds
        left_motor_power = std::max(std::min(left_motor_power, max_power), -max_power);
        right_motor_power = std::max(std::min(right_motor_power, max_power), -max_power);
    }

    // Debugging: Print the pixel map to console
    void printPixelMap() {
        for (const auto& row : pixel_map) {
            for (const auto& cell : row) {
                std::cout << (cell > 0.0 ? "#" : "."); // "#" for obstacle, "." for empty space
            }
            std::cout << std::endl;
        }
    }
};