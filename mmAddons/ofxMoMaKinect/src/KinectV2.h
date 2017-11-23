#ifndef _KINECT_V2_H_
#define _KINECT_V2_H_

#include <Kinect.h>
#include <Kinect.Face.h>
/*Don't forget to add 
C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\Redist\Face\x86
(or x64)
in the path variable in order to use face tracking.
TO CHECK: Even if you don't use it, you must add the folder in order to have this code to work properly*/


namespace KINECTV2
{
    static const int
        LOCAL_DepthWidth( 512 ),
        LOCAL_DepthHeight( 424 ),
        LOCAL_ColorWidth( 1920 ),
        LOCAL_ColorHeight( 1080 );

    static const bool
        LOCAL_IsColorImageUsed = false,
        LOCAL_IsDepthImageUsed = false,
        LOCAL_IsBodyUsed = true,
        LOCAL_IsBodyIndexImageUsed = false,
        LOCAL_IsInfraredImageUsed = false,
        LOCAL_IsLongExposuredInfraredImageUsed = false,
        LOCAL_IsAudioUsed = false,
        LOCAL_IsMapperColorFrameToCameraSpaceUsed = false,
        LOCAL_IsMapperDepthFrameToCameraSpaceUsed = false,
        LOCAL_IsLowDefinitionFaceTrackingUsed = false,
        LOCAL_IsHighDefinitionFaceTrackingUsed = false;
    //if you switch LOCAL_IsHightDefinitionFaceTrackingUsed to true, don't forget
    //to enable the color, the depth and the infrared.

    static const DWORD LOCAL_FaceFrameFeatures =
        FaceFrameFeatures::FaceFrameFeatures_BoundingBoxInColorSpace
        | FaceFrameFeatures::FaceFrameFeatures_BoundingBoxInInfraredSpace
        | FaceFrameFeatures::FaceFrameFeatures_PointsInInfraredSpace
        | FaceFrameFeatures::FaceFrameFeatures_PointsInColorSpace
        | FaceFrameFeatures::FaceFrameFeatures_RotationOrientation
        | FaceFrameFeatures::FaceFrameFeatures_Happy
        | FaceFrameFeatures::FaceFrameFeatures_RightEyeClosed
        | FaceFrameFeatures::FaceFrameFeatures_LeftEyeClosed
        | FaceFrameFeatures::FaceFrameFeatures_MouthOpen
        | FaceFrameFeatures::FaceFrameFeatures_MouthMoved
        | FaceFrameFeatures::FaceFrameFeatures_LookingAway
        | FaceFrameFeatures::FaceFrameFeatures_Glasses
        | FaceFrameFeatures::FaceFrameFeatures_FaceEngagement;

    struct JOINT_STRUCT
    {
        CameraSpacePoint
            Joint;
		Vector4
			JointOrientation;
        ColorSpacePoint
            JointColor;
        DepthSpacePoint
            JointDepth;
        TrackingState
            JointTrackingState;
    };

    struct BODY_STRUCT
    {
        BOOLEAN
            IsTracked;
        UINT64
            TrackingId;
        PointF
            Lean;
        TrackingState
            LeanTrackingState;
        JOINT_STRUCT
            Joints[ JointType_Count ];
        HandState
            HandLeftState;
        HandState
            HandRightState;
    };

    struct LOW_DEFINITION_FACE_STRUCT
    {
        BOOLEAN
            IsTracked;
        RectI
            BoundingBoxInColorSpace,
            BoundingBoxInInfraredSpace;
        PointF
            PointsInColorSpace[ FacePointType::FacePointType_Count ],
            PointsInInfraredSpace[ FacePointType::FacePointType_Count ];
        Vector4
            FaceRotation;
        DetectionResult
            Properties[ FaceProperty::FaceProperty_Count ];

    };

    struct HIGH_DEFINITION_FACE_STRUCT
    {
        BOOLEAN
            IsTracked;
        Vector4
            FaceRotation;
        CameraSpacePoint
            HeadPivotPoint;

        float
            FaceShapeAnimations[ _FaceShapeAnimations::FaceShapeAnimations_Count ];
    };

    class KinectV2
    {

        public:

            // CONSTRUCTORS

            KinectV2(
                void
                );

            // DESTRUCTORS

            ~KinectV2(
                void
                );

            // ACCESSORS

            BOOLEAN GetIsKinectAvailable(
                void
                );

            //~~

            UINT GetDepthImageBufferSize(
                void
                );

            //~~

            UINT16 * GetDepthImageBuffer(
                void
                );

            //~~

            RGBQUAD * GetColorImageBuffer(
                void
                );

            //~~

            BODY_STRUCT * GetBodies(
                void
                );

            //~~

            BYTE * GetBodyIndexImageBuffer(
                void
                );

            //~~

            int GetBodiesSize(
                void
                );

            //~~

            CameraSpacePoint * GetColorCoordinatesInCameraSpace(
                void
                );

            //~~

            CameraSpacePoint * GetDepthCoordinatesInCameraSpace(
                void
                );

            //~~

            LOW_DEFINITION_FACE_STRUCT * GetLowDefinitionFaceStruct(
                void
                );

            //~~

            HIGH_DEFINITION_FACE_STRUCT * GetHighDefinitionFaceStruct(
                void
                );

            //~~

            int GetUserTrackedCount(
                void
                );

            //~~

            Vector4 GetFloorClipPlane(
                void
                );

            //~~

            INT64 GetColorRelativeTime(
                void
                );

            //~~

            INT64 GetDepthRelativeTime(
                void
                );

            //~~

            INT64 GetBodiesRelativeTime(
                void
                );

            //~~

            INT64 GetBodyIndexRelativeTime(
                void
                );

            // MUTATORS

            // FUNCTIONS

            HRESULT Initialize(
                void
                );

            //~~

            void Finalize(
                void
                );

            //~~

            HRESULT Update(
                void
                );

            //~~

            HRESULT ProcessDepthImage(
                void
                );

            //~~

            HRESULT ProcessColorImage(
                void
                );

            //~~

            HRESULT ProcessBodies(
                void
                );

            //~~

            HRESULT ProcessBodiesIndexImage(
                void
                );

            //~~

            HRESULT ProcessMapColorFrameToCameraSpace(
                void
                );

            //~~

            HRESULT ProcessMapDepthFrameToCameraSpace(
                void
                );

            //~~

            void ProcessLowDefinitionFaces(
                void
                );

            //~~

            void ProcessHighDefinitionFaces(
                void
                );
			inline int getUsersTrackedCount(){return
				UsersTrackedCount;
			}
                
        private:

            // ATTRIBUTES

            IKinectSensor
                * KinectSensor;
            ICoordinateMapper
                * CoordinateMapper;
            IBody
                * Bodies[ BODY_COUNT ];
            IMultiSourceFrameReader
                * MultiSourceFrameReader;
            IMultiSourceFrame
                * MultiSourceFrame;
            IFaceFrameSource
                * LowDefinitionFaceFrameSources[ BODY_COUNT ];
            IFaceFrameReader
                * LowDefinitionFaceFrameReaders[ BODY_COUNT ];
            IHighDefinitionFaceFrameSource
                * HighDefinitionFaceFrameSources[ BODY_COUNT ];
            IHighDefinitionFaceFrameReader
                * HighDefinitionFaceFrameReaders[ BODY_COUNT ];
            LOW_DEFINITION_FACE_STRUCT
                LowDefinitionFaceStruct[ BODY_COUNT ];
            HIGH_DEFINITION_FACE_STRUCT
                HighDefinitionFaceStruct[ BODY_COUNT ];
            CameraSpacePoint
                * ColorCoordinatesInCameraSpace,
                * DepthCoordinatesInCameraSpace;
            RGBQUAD
                * ColorImageBuffer;
            UINT16
                * DepthImageBuffer;
            UINT
                DepthImageBufferSize,
                BodyIndexImageBufferSize,
                ColorImageBufferSize;
            INT64
                DepthFrameRelativeTime,
                ColorFrameRelativeTime,
                BodiesFrameRelativeTime,
                BodyIndexFrameRelativeTime;
            
            BYTE
                * BodyIndexImageBuffer;
            BODY_STRUCT
                BodiesStruct[ BODY_COUNT ];
            bool
                IsMultiSourceUsed;
            int
                UsersTrackedCount;
            Vector4
                FloorClipPlane;
    };
}

#endif //_KINECT_V2_H_