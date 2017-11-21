#include "mmKinematics.h"
#include "mmGeometry.h"
#include "mmSignal.h"

using namespace arma;
using namespace std;
using namespace MoMa;



mat MoMa::speedm(const Trace &trace) {

    return speedm(trace.position);
}

mat MoMa::accelerationm(const Trace &trace) {

    return accelerationm(trace.position);
}

vec MoMa::normSpeedm(const Trace &trace) {

    return normSpeedm(trace.position);
}

vec MoMa::normAccelerationm(const Trace &trace) {

    return normAccelerationm(trace.position);
}

mat MoMa::speedm(const TimedMat &data) {
    
    return derivate(data.getData(), data.frameRate());
}

mat MoMa::accelerationm(const TimedMat &data) {

    mat s = derivate(data.getData(), data.frameRate());
    return derivate(s, data.frameRate());
}

vec MoMa::normSpeedm(const TimedMat &data) {

    mat s = derivate(data.getData(), data.frameRate());
    return norm_col(s);
}

vec MoMa::normAccelerationm(const TimedMat &data) {

    mat s = derivate(data.getData(), data.frameRate());
    mat a = derivate(s, data.frameRate());
    return norm_col(a);
}

mat MoMa::derivate(const arma::mat &data, const double frameRate, int type) {
    
    int n = data.n_cols;
    mat d;
        d.set_size(data.n_rows,n);

    if (type == 0) {

        if (n < 5) {

            d.zeros();

            cout << "Data too small for five point derivation (at least 5 frames needed)." << endl;

            return d;
        }

        //Five-point version
        d.cols(2, n - 3) = -data.cols(4, n - 1) + 8 * data.cols(3, n - 2) - 8 * data.cols(1, n - 4) + data.cols(0, n - 5);
        d.col(1) = d.col(2);
        d.col(0) = d.col(2);
        d.col(n - 2) = d.col(n - 3);
        d.col(n - 1) = d.col(n - 3);
        float h = 1.0f / frameRate;
        d /= 12 * h;
    }
    if(type == 1) {

        //Asymmetric version
        d.cols(1,n-1) = data.cols( 1, n-1 );
        d.cols(1,n-1) -= data.cols(0, n-2 );
        d.col(0) = d.col(1);
        d *= frameRate;
    }
    else if(type == 2){

        //Symmetric version
        d.cols(1,n-2) = data.cols( 2, n-1 );
        d.cols(1,n-2) -= data.cols(0, n-3 );
        d.col(0) = d.col(1);
        d.col(n-1) = d.col(n-2);
        d *= 2*frameRate;
    }
    return d;
}

vec MoMa::derivate(const arma::vec &data, const double frameRate, int type) {
    
    int n = data.n_rows;
    vec d(n);
    
    if (type == 0) {

        if (n < 5) {

            d.zeros();

            cout << "Data too small for five point derivation (at least 5 frames needed)." << endl;

            return d;
        }

        //Five-point version
        d.rows(2, n - 3) = -data.rows(4, n - 1) + 8 * data.rows(3, n - 2) - 8 * data.rows(1, n - 4) + data.rows(0, n - 5);
        d.row(1) = d.row(2);
        d.row(0) = d.row(2);
        d.row(n - 2) = d.row(n - 3);
        d.row(n - 1) = d.row(n - 3);
        float h = 1.0f / frameRate;
        d /= 12 * h;
    }
    if (type == 1) {

        //Asymmetric version
        d.rows(1, n - 1) = data.cols(1, n - 1);
        d.rows(1, n - 1) -= data.cols(0, n - 2);
        d.row(0) = d.row(1);
        d *= frameRate;
    }
    else if (type == 2) {

        //Symmetric version
        d.rows(1, n - 2) = data.cols(2, n - 1);
        d.rows(1, n - 2) -= data.cols(0, n - 3);
        d.row(0) = d.row(1);
        d.row(n - 1) = d.row(n - 2);
        d *= 2 * frameRate;
    }
    return d;
}

mat MoMa::derivate(const arma::mat &data, const arma::vec &timeVec, int type) {

    int n = data.n_cols;
    mat d;
    d.set_size(data.n_rows, n);
    vec h(n);


    if (type == 0) {

        type = 2; //No five-point derivation for timed data
    }
    if (type == 1) {

        //Asymmetric version
        d.cols(1, n - 1) = data.cols(1, n - 1);
        d.cols(1, n - 1) -= data.cols(0, n - 2);
        
        h(1, n - 1) = timeVec(1, n - 1);
        h(1, n - 1) -= timeVec(0, n - 2);
        
        d.each_row() /= h.t();

        d.col(0) = d.col(1);
    }
    else if (type == 2) {

        //Symmetric version
        d.cols(1, n - 2) = data.cols(2, n - 1);
        d.cols(1, n - 2) -= data.cols(0, n - 3);

        h(1, n - 2) = timeVec(2, n - 1);
        h(1, n - 2) -= timeVec(0, n - 3);

        d.each_row() /= h.t();

        d.col(0) = d.col(1);
        d.col(n - 1) = d.col(n - 2);
    }
    return d;
}

vec MoMa::derivate(const arma::vec &data, const arma::vec &timeVec, int type) {

    int n = data.n_cols;
    mat d;
    d.set_size(data.n_rows, n);
    vec h(n);


    if (type == 0) {

        type = 2; //No five-point derivation for timed data
    }
    if (type == 1) {

        //Asymmetric version
        d.rows(1, n - 1) = data.rows(1, n - 1);
        d.rows(1, n - 1) -= data.rows(0, n - 2);

        h(1, n - 1) = timeVec(1, n - 1);
        h(1, n - 1) -= timeVec(0, n - 2);

        d /= h;

        d.row(0) = d.row(1);
    }
    else if (type == 2) {

        //Symmetric version
        d.rows(1, n - 2) = data.rows(2, n - 1);
        d.rows(1, n - 2) -= data.rows(0, n - 3);

        h(1, n - 2) = timeVec(2, n - 1);
        h(1, n - 2) -= timeVec(0, n - 3);

        d /= h;

        d.row(0) = d.row(1);
        d.row(n - 1) = d.row(n - 2);
    }
    return d;
}



vec MoMa::norm_col(const arma::mat &data) {

    return sqrt(sum(data%data)).t(); // calculate the norm of each column and stores it in a rowvec, that we translate in a vec
}

TimedMat MoMa::derivate(const MoMa::TimedMat &data, int type) {
    
    if(!data.isTimed())
        return TimedMat(data.frameRate(), derivate(data.getData(), data.frameRate(), type), data.initialTime());
    else
        return TimedMat(data.getTimeVec(), derivate(data.getData(), data.getTimeVec(), type));
}
TimedVec MoMa::derivate(const MoMa::TimedVec &data, int type) {

    if (!data.isTimed())
        return TimedVec(data.frameRate(), derivate(data.getData(), data.frameRate(), type), data.initialTime());
    else
        return TimedVec(data.getTimeVec(), derivate(data.getData(), data.getTimeVec(), type));
}
TimedVec MoMa::norm(const MoMa::TimedMat &data) {

    if (!data.isTimed())
        return TimedVec(data.frameRate(), norm_col(data.getData()), data.initialTime());
    else
        return TimedVec(data.getTimeVec(), norm_col(data.getData()));
}
TimedVec MoMa::abs(const MoMa::TimedVec &data) {

    if (!data.isTimed())
        return TimedVec(data.frameRate(), abs(data.getData()), data.initialTime());
    else
        return TimedVec(data.getTimeVec(), abs(data.getData()));
}
TimedMat MoMa::speed(const MoMa::Trace &trace) {

    return speed(trace.position);

}
TimedMat MoMa::acceleration(const MoMa::Trace &trace) {

    return acceleration(trace.position);
}
TimedVec MoMa::normSpeed(const MoMa::Trace &trace) {

    return normSpeed(trace.position);
}
TimedVec MoMa::normAcceleration(const MoMa::Trace &trace) {

    return normAcceleration(trace.position);
}

TimedMat MoMa::speed(const MoMa::TimedMat &data) {

    return derivate(data);
}
TimedMat MoMa::acceleration(const MoMa::TimedMat &data) {

    return derivate(derivate(data));
}
TimedVec MoMa::speed(const MoMa::TimedVec &data) {

    return derivate(data);
}
TimedVec MoMa::acceleration(const MoMa::TimedVec &data) {

    return derivate(derivate(data));
}
TimedVec MoMa::normSpeed(const MoMa::TimedMat &data) {

    return norm(speed(data));
}
TimedVec MoMa::normAcceleration(const MoMa::TimedMat &data) {

    return norm(acceleration(data));
}

float MoMa::meanFemurLength(const MoMa::Track &tr) {

    vec flPerFrame;
    const cube &data(tr.position.getData());


    if (tr.hasNodeList) {

        int LHip = tr.nodeList->index("LHip");
        int RHip = tr.nodeList->index("RHip");
        int LKnee = tr.nodeList->index("LKnee");
        int RKnee = tr.nodeList->index("RKnee");

        //No need to process all frames!
        int hop = data.n_slices / 20;
        hop = max(hop, 1);
        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;
            flPerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            flPerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(LKnee))
                + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(RKnee))) / 2.0f;
        }
    }
    else {

        cout << "WARNING (MoMa::meanFemurLength): The Track needs a nodeList containing the necessary nodes to compute femur length." << endl;
        return arma::datum::nan;
    }

    return(nanmean(flPerFrame));
}

float MoMa::meanSizeToArm(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LHip = tr.nodeList->index("LHip");
        int RHip = tr.nodeList->index("RHip");
        int LKnee = tr.nodeList->index("LKnee");
        int RKnee = tr.nodeList->index("RKnee");
        int LAnkle = tr.nodeList->index("LAnkle");
        int RAnkle = tr.nodeList->index("RAnkle");
        int Thorax = tr.nodeList->index("Thorax");
        int Neck = tr.nodeList->index("Neck");
        int Head = tr.nodeList->index("Head");
        int LShoulder = tr.nodeList->index("LShoulder");
        int RShoulder = tr.nodeList->index("RShoulder");
        int LElbow = tr.nodeList->index("LElbow");
        int RElbow = tr.nodeList->index("RElbow");
        int LWrist = tr.nodeList->index("LWrist");
        int RWrist = tr.nodeList->index("RWrist");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(RKnee))) / 2.0f //thigh
                + (Geometry::distance(sliceT.unsafe_col(LAnkle), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RAnkle), sliceT.unsafe_col(RKnee))) / 2.0f //shank
                + (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(Thorax)) + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(Thorax))) / 2.0f //trunk1
                + Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(Thorax)) //trunk2
                + (Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(LShoulder)) + Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(RShoulder))) / 2.0f //scapula
                + (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LShoulder)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RShoulder))) / 2.0f //arm
                + (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LWrist)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RWrist))) / 2.0f; //forearm
        }
    }
    else {

        cout << "WARNING (MoMa::meanSizeToArm): The Track needs a nodeList containing the necessary nodes to compute mean size to arm." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanArmSize(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {
       
        int LShoulder = tr.nodeList->index("LShoulder");
        int RShoulder = tr.nodeList->index("RShoulder");
        int LElbow = tr.nodeList->index("LElbow");
        int RElbow = tr.nodeList->index("RElbow");
        int LWrist = tr.nodeList->index("LWrist");
        int RWrist = tr.nodeList->index("RWrist");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LShoulder)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RShoulder))) / 2.0f //arm
                + (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LWrist)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RWrist))) / 2.0f; //forearm
        }
    }
    else {

        cout << "WARNING (MoMa::meanArmSize): The Track needs a nodeList containing the necessary nodes to compute mean arm size." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanTrunkSize(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int Pelvis = tr.nodeList->index("Pelvis");
        int Thorax = tr.nodeList->index("Thorax");
        int Neck = tr.nodeList->index("Neck");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = Geometry::distance(sliceT.unsafe_col(Pelvis), sliceT.unsafe_col(Thorax)) //trunk1
                + Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(Thorax)); //trunk2
        }
    }
    else {

        cout << "WARNING (MoMa::meanTrunkSize): The Track needs a nodeList containing the necessary nodes to compute mean trunk size." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanLegSize(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LHip = tr.nodeList->index("LHip");
        int RHip = tr.nodeList->index("RHip");
        int LKnee = tr.nodeList->index("LKnee");
        int RKnee = tr.nodeList->index("RKnee");
        int LAnkle = tr.nodeList->index("LAnkle");
        int RAnkle = tr.nodeList->index("RAnkle");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(RKnee))) / 2.0f //thigh
                + (Geometry::distance(sliceT.unsafe_col(LAnkle), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RAnkle), sliceT.unsafe_col(RKnee))) / 2.0f; //shank
        }
    }
    else {

        cout << "WARNING (MoMa::meanLegSize): The Track needs a nodeList containing the necessary nodes to compute mean leg size." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanFootSize(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LAnkle = tr.nodeList->index("LAnkle");
        int RAnkle = tr.nodeList->index("RAnkle");
        int LFoot = tr.nodeList->index("LFoot");
        int RFoot = tr.nodeList->index("RFoot");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LAnkle), sliceT.unsafe_col(LFoot)) + Geometry::distance(sliceT.unsafe_col(RAnkle), sliceT.unsafe_col(RFoot))) / 2.0f; //foot
        }
    }
    else {

        cout << "WARNING (MoMa::meanFootSize): The Track needs a nodeList containing the necessary nodes to compute mean foot size." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanHumerusLength(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LShoulder = tr.nodeList->index("LShoulder");
        int RShoulder = tr.nodeList->index("RShoulder");
        int LElbow = tr.nodeList->index("LElbow");
        int RElbow = tr.nodeList->index("RElbow");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LShoulder)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RShoulder))) / 2.0f;
        }
    }
    else {

        cout << "WARNING (MoMa::meanHumerusLength): The Track needs a nodeList containing the necessary nodes to compute mean humerus length." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanShoulderWidth(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LShoulder = tr.nodeList->index("LShoulder");
        int RShoulder = tr.nodeList->index("RShoulder");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = Geometry::distance(sliceT.unsafe_col(RShoulder), sliceT.unsafe_col(LShoulder));
        }
    }
    else {

        cout << "WARNING (MoMa::meanShoulderWidth): The Track needs a nodeList containing the necessary nodes to compute mean shoulder width." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanHipWidth(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LHip = tr.nodeList->index("LHip");
        int RHip = tr.nodeList->index("RHip");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(LHip));
        }
    }
    else {

        cout << "WARNING (MoMa::meanHipWidth): The Track needs a nodeList containing the necessary nodes to compute hip width." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));
}

float MoMa::meanForearmLength(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LWrist = tr.nodeList->index("LWrist");
        int RWrist = tr.nodeList->index("RWrist");
        int LElbow = tr.nodeList->index("LElbow");
        int RElbow = tr.nodeList->index("RElbow");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LElbow), sliceT.unsafe_col(LWrist)) + Geometry::distance(sliceT.unsafe_col(RElbow), sliceT.unsafe_col(RWrist))) / 2.0f;
        }
    }
    else {

        cout << "WARNING (MoMa::meanForearmLength): The Track needs a nodeList containing the necessary nodes to compute mean forearm length." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));

}

float MoMa::meanShankLength(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LAnkle = tr.nodeList->index("LAnkle");
        int RAnkle = tr.nodeList->index("RAnkle");
        int LKnee = tr.nodeList->index("LKnee");
        int RKnee = tr.nodeList->index("RKnee");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LAnkle), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RAnkle), sliceT.unsafe_col(RKnee))) / 2.0f;
        }
    }
    else {

        cout << "WARNING (MoMa::meanShankLength): The Track needs a nodeList containing the necessary nodes to compute mean shank length." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));

}

float MoMa::meanHeadHeight(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int Neck = tr.nodeList->index("Neck");
        int Head = tr.nodeList->index("Head");

        //No need to process all frames!
        int hop = data.n_slices / 100;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;

            sizePerFrame.resize(j + 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(Head));
        }
    }
    else {

        cout << "WARNING (MoMa::meanHeadHeight): The Track needs a nodeList containing the necessary nodes to compute mean head height." << endl;
        return arma::datum::nan;
    }

    return(nanmean(sizePerFrame));

}

float MoMa::meanSize(const Track &tr) {

    vec sizePerFrame;
    const cube &data = tr.position.getData();


    if (tr.hasNodeList) {

        int LHip = tr.nodeList->index("LHip");
        int RHip = tr.nodeList->index("RHip");
        int LKnee = tr.nodeList->index("LKnee");
        int RKnee = tr.nodeList->index("RKnee");
        int LAnkle = tr.nodeList->index("LAnkle");
        int RAnkle = tr.nodeList->index("RAnkle");
        int Thorax = tr.nodeList->index("Thorax");
        int Neck = tr.nodeList->index("Neck");
        int Head = tr.nodeList->index("Head");

        //No need to process all frames!
        int hop = data.n_slices / 20;
        hop = max(hop, 1);

        int j = 0;

        for (int t = 0; t < data.n_slices; t = t + hop) {

            ++j;
            sizePerFrame.resize(j + 1, 1);

            const mat sliceT = data.slice(t);

            sizePerFrame(j) = (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(RKnee))) / 2.0f //thigh
                + (Geometry::distance(sliceT.unsafe_col(LAnkle), sliceT.unsafe_col(LKnee)) + Geometry::distance(sliceT.unsafe_col(RAnkle), sliceT.unsafe_col(RKnee))) / 2.0f //shank
                + (Geometry::distance(sliceT.unsafe_col(LHip), sliceT.unsafe_col(Thorax)) + Geometry::distance(sliceT.unsafe_col(RHip), sliceT.unsafe_col(Thorax))) / 2.0f //trunk1
                + Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(Thorax)) //trunk2
                + Geometry::distance(sliceT.unsafe_col(Neck), sliceT.unsafe_col(Head)); //head
        }
    }

    return(nanmean(sizePerFrame));
}