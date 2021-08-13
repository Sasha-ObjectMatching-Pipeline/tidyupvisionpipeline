#ifndef DETECTED_OBJECT_H
#define DETECTED_OBJECT_H

#include <unordered_set>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/PointIndices.h>
#include <pcl/filters/filter.h>

#include "plane_object_extraction.h"

typedef pcl::PointXYZRGBNormal PointNormal;

const float max_dist_for_being_static = 0.1; //how much can the object be displaced to still count as static

enum ObjectState {NEW, REMOVED, DISPLACED, STATIC, UNKNOWN};

struct FitnessScoreStruct {
    FitnessScoreStruct() { object_conf = 0.0f; model_conf = 0.0f;}
    FitnessScoreStruct(float o_c, float m_c, Eigen::Array<bool, Eigen::Dynamic, 1> o_pts, Eigen::Array<bool, Eigen::Dynamic, 1> m_pts ) :
        object_conf{o_c}, model_conf{m_c}, object_overlapping_pts{o_pts}, model_overlapping_pts{m_pts} {}
    float object_conf;
    float model_conf;
    Eigen::Array<bool, Eigen::Dynamic, 1> object_overlapping_pts;
    Eigen::Array<bool, Eigen::Dynamic, 1> model_overlapping_pts;
};

struct Match {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Eigen::Matrix<float,4,4,Eigen::DontAlign> transform;
    int model_id;
    int object_id;
    float confidence;
    FitnessScoreStruct fitness_score;

    Match() {model_id = -1; object_id = -1, confidence = -1; transform = Eigen::Matrix4f::Identity();}
    Match(int m_id, int o_id, float conf, Eigen::Matrix4f t, FitnessScoreStruct f=FitnessScoreStruct()) {
        model_id = m_id; object_id = o_id; confidence = conf; transform = Eigen::Matrix4f(t); fitness_score = f;
    }
};

struct DetectedObject {


public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    DetectedObject(){}

    DetectedObject(pcl::PointCloud<PointNormal>::Ptr object_cloud, pcl::PointCloud<PointNormal>::Ptr plane_cloud,
                   PlaneStruct supp_plane, ObjectState object_state = UNKNOWN, std::string object_folder_path = "") : unique_id_(++s_id), object_cloud_(object_cloud),
        plane_cloud_(plane_cloud), supp_plane_(supp_plane), state_(object_state), object_folder_path_(object_folder_path) {
        std::vector<int> nan_ind;
        pcl::removeNaNFromPointCloud(*object_cloud_, *object_cloud_, nan_ind);
    }

    Match match_; //this is only relevant for displaced objects
    pcl::PointCloud<PointNormal>::Ptr plane_cloud_;
    PlaneStruct supp_plane_;
    std::string object_folder_path_;
    ObjectState state_;
    std::unordered_set<int> already_checked_model_ids;

    int getID() const {return unique_id_;}
    void setObjectCloud(pcl::PointCloud<PointNormal>::Ptr object_cloud) {
        object_cloud_ = object_cloud;
        std::vector<int> nan_ind;
        pcl::removeNaNFromPointCloud(*object_cloud_, *object_cloud_, nan_ind);
    }
    pcl::PointCloud<PointNormal>::Ptr getObjectCloud() const {return object_cloud_;}
    
protected:
    static int s_id;

private:
    int unique_id_;
    pcl::PointCloud<PointNormal>::Ptr object_cloud_;
};

#endif // DETECTED_OBJECT_H
