#include "KinectV2.h"

// CONSTRUCTORS

KINECTV2::KinectV2::KinectV2(
    void
    ) :
    KinectSensor( nullptr ),
    CoordinateMapper( nullptr ),
    Bodies(),
    MultiSourceFrameReader( nullptr ),
    MultiSourceFrame( nullptr ),
    LowDefinitionFaceFrameSources(),
    LowDefinitionFaceFrameReaders(),
    HighDefinitionFaceFrameSources(),
    HighDefinitionFaceFrameReaders(),
    LowDefinitionFaceStruct(),
    HighDefinitionFaceStruct(),
    ColorCoordinatesInCameraSpace( nullptr ),
    DepthCoordinatesInCameraSpace( nullptr ),
    ColorImageBuffer( nullptr ),
    DepthImageBuffer( nullptr ),
    DepthImageBufferSize( LOCAL_DepthHeight * LOCAL_DepthWidth ),
    BodyIndexImageBufferSize( LOCAL_DepthHeight * LOCAL_DepthWidth ),
    ColorImageBufferSize( LOCAL_ColorHeight * LOCAL_ColorWidth ),
    BodyIndexImageBuffer( nullptr ),
    BodiesStruct(),
    IsMultiSourceUsed( false ),
    UsersTrackedCount( 0 ),
    FloorClipPlane()
{
    int
        body_index;

    this->DepthImageBuffer = new UINT16[ LOCAL_DepthWidth * LOCAL_DepthHeight ];
    this->ColorImageBuffer = new RGBQUAD[ LOCAL_ColorWidth * LOCAL_ColorHeight ];
    this->BodyIndexImageBuffer = new BYTE[ LOCAL_DepthWidth * LOCAL_DepthHeight ];

    this->ColorCoordinatesInCameraSpace = new CameraSpacePoint[ LOCAL_ColorWidth * LOCAL_ColorHeight ];
    this->DepthCoordinatesInCameraSpace = new CameraSpacePoint[ LOCAL_DepthWidth * LOCAL_DepthHeight ];

    for ( body_index = 0; body_index < BODY_COUNT; ++body_index )
    {
        this->Bodies[ body_index ] = nullptr;
        this->LowDefinitionFaceFrameReaders[ body_index ] = nullptr;
        this->LowDefinitionFaceFrameSources[ body_index ] = nullptr;
        this->HighDefinitionFaceFrameReaders[ body_index ] = nullptr;
        this->HighDefinitionFaceFrameSources[ body_index ] = nullptr;
    }
}

// DESTRUCTORS

KINECTV2::KinectV2::~KinectV2(
    void
    )
{
}

// ACCESSORS

BOOLEAN KINECTV2::KinectV2::GetIsKinectAvailable(
    void
    )
{
    BOOLEAN
        is_available;
	if (this->KinectSensor) {
		this->KinectSensor->get_IsAvailable(&is_available);

		return is_available;
	}
	return -1;
}

//~~

UINT KINECTV2::KinectV2::GetDepthImageBufferSize(
    void
    )
{
    return this->DepthImageBufferSize;
}

//~~

UINT16 * KINECTV2::KinectV2::GetDepthImageBuffer(
    void
    )
{
    return this->DepthImageBuffer;
}

//~~

RGBQUAD * KINECTV2::KinectV2::GetColorImageBuffer(
    void
    )
{
    return this->ColorImageBuffer;
}

//~~

KINECTV2::BODY_STRUCT * KINECTV2::KinectV2::GetBodies(
    void
    )
{
    return this->BodiesStruct;
}

//~~

BYTE * KINECTV2::KinectV2::GetBodyIndexImageBuffer(
    void
    )
{
    return this->BodyIndexImageBuffer;
}

//~~

int KINECTV2::KinectV2::GetBodiesSize(
    void
    )
{
    return BODY_COUNT;
}

//~~

CameraSpacePoint * KINECTV2::KinectV2::GetColorCoordinatesInCameraSpace(
    void
    )
{
    return this->ColorCoordinatesInCameraSpace;
}

//~~

CameraSpacePoint * KINECTV2::KinectV2::GetDepthCoordinatesInCameraSpace(
    void
    )
{
    return this->DepthCoordinatesInCameraSpace;
}

//~~

KINECTV2::LOW_DEFINITION_FACE_STRUCT * KINECTV2::KinectV2::GetLowDefinitionFaceStruct(
    void
    )
{
    return this->LowDefinitionFaceStruct;
}

//~~

KINECTV2::HIGH_DEFINITION_FACE_STRUCT * KINECTV2::KinectV2::GetHighDefinitionFaceStruct(
    void
    )
{
    return this->HighDefinitionFaceStruct;
}

//~~

int KINECTV2::KinectV2::GetUserTrackedCount(
    void
    )
{
    return this->UsersTrackedCount;
}

//~~

Vector4 KINECTV2::KinectV2::GetFloorClipPlane(
    void
    )
{
    return this->FloorClipPlane;
}

//~~

INT64 KINECTV2::KinectV2::GetColorRelativeTime(
    void
    )
{
    return this->ColorFrameRelativeTime;
}

//~~

INT64 KINECTV2::KinectV2::GetDepthRelativeTime(
    void
    )
{
    return this->DepthFrameRelativeTime;
}

//~~

INT64 KINECTV2::KinectV2::GetBodiesRelativeTime(
    void
    )
{
    return this->BodiesFrameRelativeTime;
}

//~~

INT64 KINECTV2::KinectV2::GetBodyIndexRelativeTime(
    void
    )
{
    return this->BodyIndexFrameRelativeTime;
}

// MUTATORS

// FUNCTIONS

HRESULT KINECTV2::KinectV2::Initialize(
    void
    )
{
    HRESULT
        result;
    DWORD
        enabled_frame_source_types( 0 );
    int
        source_type_used_count( 0 ),
        body_index( 0 );

    result = GetDefaultKinectSensor( & this->KinectSensor );

    if ( SUCCEEDED( result ) )
    {
        result = this->KinectSensor->Open();
    }
    
    if ( SUCCEEDED( result ) && ( LOCAL_IsMapperColorFrameToCameraSpaceUsed || LOCAL_IsMapperDepthFrameToCameraSpaceUsed || LOCAL_IsBodyUsed) )
    {
        result = this->KinectSensor->get_CoordinateMapper( & this->CoordinateMapper );
    }

    if ( SUCCEEDED( result ) )
    {
        if ( LOCAL_IsColorImageUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_Color;
            ++source_type_used_count;
        }
        
        if ( LOCAL_IsDepthImageUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_Depth;
            ++source_type_used_count;
        }
        
        if ( LOCAL_IsBodyUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_Body;
            ++source_type_used_count;
        }
        
        if ( LOCAL_IsBodyIndexImageUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_BodyIndex;
            ++source_type_used_count;
        }

        if ( LOCAL_IsInfraredImageUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_Infrared;
            ++source_type_used_count;
        }

        if ( LOCAL_IsLongExposuredInfraredImageUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_LongExposureInfrared;
            ++source_type_used_count;
        }

        if ( LOCAL_IsAudioUsed )
        {
            enabled_frame_source_types |= FrameSourceTypes::FrameSourceTypes_Audio;
            ++source_type_used_count;
        }

        if ( source_type_used_count > 1 )
        {
            this->IsMultiSourceUsed = true;
        }
        else
        {
            this->IsMultiSourceUsed = false;
        }
    }

   // if ( this->IsMultiSourceUsed )//to uncomment when the monosource will be implemented
    {
        if ( SUCCEEDED( result ) )
        {
            result = this->KinectSensor->OpenMultiSourceFrameReader(
                enabled_frame_source_types,
                & this->MultiSourceFrameReader );
        }
    }
    //else//to uncomment when the monosource will be implemented
    {
        //TODO
    }

    if ( SUCCEEDED( result ) && LOCAL_IsLowDefinitionFaceTrackingUsed )
    {
        for ( body_index = 0; body_index < BODY_COUNT; ++body_index )
        {
            if ( SUCCEEDED( result ) )
            {
                result = CreateFaceFrameSource( this->KinectSensor, 0, LOCAL_FaceFrameFeatures, & this->LowDefinitionFaceFrameSources[ body_index ] );
            }

            if ( SUCCEEDED( result ) )
            {
                result = this->LowDefinitionFaceFrameSources[ body_index ]->OpenReader( & this->LowDefinitionFaceFrameReaders[ body_index ] );
            }
        }
    }

    if ( SUCCEEDED( result ) && LOCAL_IsHighDefinitionFaceTrackingUsed )
    {

        for ( body_index = 0; body_index < BODY_COUNT; ++body_index )
        {
            if ( SUCCEEDED( result ) )
            {
                result = CreateHighDefinitionFaceFrameSource( this->KinectSensor, & this->HighDefinitionFaceFrameSources[ body_index ] );
            }

            if ( SUCCEEDED( result ) )
            {
                result = this->HighDefinitionFaceFrameSources[ body_index ]->OpenReader( & this->HighDefinitionFaceFrameReaders[ body_index ] );
            }
        }
    }

    return result;
}

//~~

void KINECTV2::KinectV2::Finalize(
    void
    )
{
    int
        body_index;

    if ( nullptr != this->DepthImageBuffer )
    {
        delete[] this->DepthImageBuffer;
        this->DepthImageBuffer = nullptr;
    }

    if ( nullptr != this->ColorImageBuffer )
    {
        delete[] this->ColorImageBuffer;
        this->ColorImageBuffer = nullptr;
    }

    if ( nullptr != this->BodyIndexImageBuffer )
    {
        delete[] this->BodyIndexImageBuffer;
        this->BodyIndexImageBuffer = nullptr;
    }

    if ( nullptr != this->MultiSourceFrameReader )
    {
        this->MultiSourceFrameReader->Release();
        this->MultiSourceFrameReader = nullptr;
    }

    if ( nullptr != this->ColorCoordinatesInCameraSpace )
    {
        delete[] this->ColorCoordinatesInCameraSpace;
        this->ColorCoordinatesInCameraSpace = nullptr;
    }

    if ( nullptr != this->DepthCoordinatesInCameraSpace )
    {
        delete[] this->DepthCoordinatesInCameraSpace;
        this->DepthCoordinatesInCameraSpace = nullptr;
    }

    for ( body_index = 0; body_index < _countof( this->Bodies ); ++body_index )
    {
        if ( this->Bodies[ body_index ] != nullptr )
        {
            this->Bodies[ body_index ]->Release();
            this->Bodies[ body_index ] = nullptr;
        }

        if ( this->LowDefinitionFaceFrameReaders[ body_index ] != nullptr )
        {
            this->LowDefinitionFaceFrameReaders[ body_index ]->Release();
            this->LowDefinitionFaceFrameReaders[ body_index ] = nullptr;
        }

        if ( this->LowDefinitionFaceFrameSources[ body_index ] != nullptr )
        {
            this->LowDefinitionFaceFrameSources[ body_index ]->Release();
            this->LowDefinitionFaceFrameSources[ body_index ] = nullptr;
        }

        if ( this->HighDefinitionFaceFrameReaders[ body_index ] != nullptr )
        {
            this->HighDefinitionFaceFrameReaders[ body_index ]->Release();
            this->HighDefinitionFaceFrameReaders[ body_index ] = nullptr;
        }

        if ( this->HighDefinitionFaceFrameSources[ body_index ] != nullptr )
        {
            this->HighDefinitionFaceFrameSources[ body_index ]->Release();
            this->HighDefinitionFaceFrameSources[ body_index ] = nullptr;
        }
    }

    if ( nullptr != this->KinectSensor )
    {
        this->KinectSensor->Close();
        this->KinectSensor->Release();
        this->KinectSensor = nullptr;
    }
}

//~~

HRESULT KINECTV2::KinectV2::Update(
    void
    )
{
    HRESULT
        result;

    result = this->MultiSourceFrameReader->AcquireLatestFrame( & this->MultiSourceFrame );
    
    if ( SUCCEEDED( result ) && LOCAL_IsDepthImageUsed )
    {
        result = ProcessDepthImage();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsColorImageUsed )
    {
        result = ProcessColorImage();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsBodyUsed )
    {
        result = ProcessBodies();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsBodyIndexImageUsed )
    {
        result = ProcessBodiesIndexImage();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsMapperColorFrameToCameraSpaceUsed )
    {
        result = ProcessMapColorFrameToCameraSpace();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsMapperDepthFrameToCameraSpaceUsed )
    {
        result = ProcessMapDepthFrameToCameraSpace();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsLowDefinitionFaceTrackingUsed )
    {
        ProcessLowDefinitionFaces();
    }

    if ( SUCCEEDED( result ) && LOCAL_IsHighDefinitionFaceTrackingUsed )
    {
        ProcessHighDefinitionFaces();
    }

    if ( this->MultiSourceFrame )
    {
        this->MultiSourceFrame->Release();
        this->MultiSourceFrame = nullptr;
    }

    return result;
}

//~~

HRESULT KINECTV2::KinectV2::ProcessDepthImage(
    void
    )
{
    IDepthFrameReference
        * depth_frame_reference( nullptr );
    IDepthFrame
        * depth_frame( nullptr );
    UINT16
        * cached_buffer( nullptr );
    HRESULT
        result;

    result = this->MultiSourceFrame->get_DepthFrameReference( &depth_frame_reference );

    if ( SUCCEEDED( result ) )
    {
        result = depth_frame_reference->AcquireFrame( &depth_frame );
    }

    if (SUCCEEDED(result))
    {
        result = depth_frame->get_RelativeTime(&DepthFrameRelativeTime);
    }
    
    if ( SUCCEEDED( result ) )
    {
        result = depth_frame->AccessUnderlyingBuffer( &this->DepthImageBufferSize, & cached_buffer );
    }

    if ( SUCCEEDED( result ) )
    {
        memcpy( this->DepthImageBuffer, cached_buffer, this->DepthImageBufferSize * sizeof( UINT16 ) );
    }

    if ( depth_frame_reference )
    {
        depth_frame_reference->Release();
        depth_frame_reference = nullptr;
    }

    if ( depth_frame )
    {
        depth_frame->Release();
        depth_frame = nullptr;
    }

    return result;
}

//~~

HRESULT KINECTV2::KinectV2::ProcessColorImage(
    void
    )
{
    IColorFrameReference
        * color_frame_reference( nullptr );
    IColorFrame
        * color_frame( nullptr );
    RGBQUAD
        * cached_buffer( nullptr );
    ColorImageFormat
        image_format( ColorImageFormat_None );
    HRESULT
        result;

    result = this->MultiSourceFrame->get_ColorFrameReference( &color_frame_reference );

    if ( SUCCEEDED( result ) )
    {
        result = color_frame_reference->AcquireFrame( & color_frame );
    }

    if (SUCCEEDED(result))
    {
        result = color_frame->get_RelativeTime(&ColorFrameRelativeTime);
    }

    if ( SUCCEEDED( result ) )
    {
        result = color_frame->get_RawColorImageFormat( & image_format );
    }
    
    if ( SUCCEEDED( result ) )
    {
        if ( image_format == ColorImageFormat_Bgra )
        {
            result = color_frame->AccessRawUnderlyingBuffer( &this->ColorImageBufferSize, reinterpret_cast< unsigned char * * >( & cached_buffer ) );

            if ( SUCCEEDED( result ) )
            {
                memcpy( this->ColorImageBuffer, cached_buffer, this->ColorImageBufferSize * sizeof( RGBQUAD ) );
            }
        }
        else
        {
            result = color_frame->CopyConvertedFrameDataToArray( this->ColorImageBufferSize * sizeof( RGBQUAD ), reinterpret_cast< unsigned char * >( this->ColorImageBuffer ), ColorImageFormat_Bgra );
        }
    }

    if ( nullptr != color_frame )
    {
        color_frame->Release();
        color_frame = nullptr;
    }

    if ( nullptr != color_frame_reference )
    {
        color_frame_reference->Release();
        color_frame_reference = nullptr;
    }
    
    return result;
}

//~~

HRESULT KINECTV2::KinectV2::ProcessBodies(
    void
    )
{
    IBodyFrame
        * body_frame( nullptr );
    IBodyFrameReference
        * body_frame_reference = nullptr;
    HRESULT
        result;
	Joint
		joints[JointType_Count];
	JointOrientation
		orientationJoints[JointType_Count];
    int
        body_index,
        joint_index;

    result = this->MultiSourceFrame->get_BodyFrameReference( &body_frame_reference );

    if ( SUCCEEDED( result ) )
    {
        result = body_frame_reference->AcquireFrame( &body_frame );
    }
    if (SUCCEEDED(result))
    {
        result = body_frame->get_RelativeTime(&BodiesFrameRelativeTime);
    }
    
    if ( SUCCEEDED( result ) )
    {
        result = body_frame->GetAndRefreshBodyData( _countof( this->Bodies ), this->Bodies );
    }

    if ( SUCCEEDED( result ) )
    {
        body_frame->get_FloorClipPlane( & this->FloorClipPlane );
        
        this->UsersTrackedCount = 0;

        for ( body_index = 0; body_index < BODY_COUNT; ++body_index )
        {
            this->Bodies[ body_index ]->get_IsTracked( & this->BodiesStruct[ body_index ].IsTracked );
            
            if ( this->BodiesStruct[ body_index ].IsTracked )
            {
                ++this->UsersTrackedCount;
            }

            this->Bodies[body_index]->get_HandLeftState( & this->BodiesStruct[body_index].HandLeftState );
            this->Bodies[body_index]->get_HandRightState(&this->BodiesStruct[body_index].HandRightState);
            this->Bodies[ body_index ]->get_TrackingId( & this->BodiesStruct[ body_index ].TrackingId );
            this->Bodies[ body_index ]->GetJoints( JointType_Count, joints );
			this->Bodies[ body_index ]->GetJointOrientations(JointType_Count, orientationJoints);
            this->Bodies[ body_index ]->get_Lean( & this->BodiesStruct[ body_index ].Lean );
            this->Bodies[ body_index ]->get_LeanTrackingState( & this->BodiesStruct[ body_index ].LeanTrackingState );

            for ( joint_index = 0; joint_index < JointType_Count; ++joint_index )
            {
                this->BodiesStruct[ body_index ].Joints[ joint_index ].JointTrackingState = joints[ joint_index ].TrackingState;
				this->BodiesStruct[body_index].Joints[joint_index].Joint = joints[joint_index].Position;
				this->BodiesStruct[body_index].Joints[joint_index].JointOrientation = orientationJoints[joint_index].Orientation;

                this->CoordinateMapper->MapCameraPointToColorSpace(joints[joint_index].Position, &this->BodiesStruct[body_index].Joints[joint_index].JointColor);
                this->CoordinateMapper->MapCameraPointToDepthSpace(joints[joint_index].Position, &this->BodiesStruct[body_index].Joints[joint_index].JointDepth);
                
            }
        }
    }

    if ( nullptr != body_frame )
    {
        body_frame->Release();
        body_frame = nullptr;
    }

    if ( nullptr != body_frame_reference )
    {
        body_frame_reference->Release();
        body_frame_reference = nullptr;
    }

    return result;
}

//~~

HRESULT KINECTV2::KinectV2::ProcessBodiesIndexImage(
    void
    )
{
    IBodyIndexFrame
        * body_index_frame( nullptr );
    IBodyIndexFrameReference
        * body_index_frame_reference( nullptr );
    BYTE
        * cached_buffer( nullptr );
    HRESULT
        result;

    result = this->MultiSourceFrame->get_BodyIndexFrameReference( &body_index_frame_reference );

    if ( SUCCEEDED( result ) )
    {
        result = body_index_frame_reference->AcquireFrame( &body_index_frame );
    }

    if (SUCCEEDED(result))
    {
        result = body_index_frame->get_RelativeTime(&BodyIndexFrameRelativeTime);
    }

    if ( SUCCEEDED( result ) )
    {
        result = body_index_frame->AccessUnderlyingBuffer( &this->BodyIndexImageBufferSize, &cached_buffer );
    }

    if ( SUCCEEDED( result ) )
    {
        memcpy( this->BodyIndexImageBuffer, cached_buffer, this->BodyIndexImageBufferSize * sizeof( BYTE ) );
    }

    if ( nullptr != body_index_frame )
    {
        body_index_frame->Release();
        body_index_frame = nullptr;
    }

    if ( nullptr != body_index_frame_reference )
    {
        body_index_frame_reference->Release();
        body_index_frame_reference = nullptr;
    }

    return result;
}

//~~

HRESULT KINECTV2::KinectV2::ProcessMapColorFrameToCameraSpace(
    void
    )
{
    return this->CoordinateMapper->MapColorFrameToCameraSpace(
        LOCAL_DepthWidth * LOCAL_DepthHeight,
        this->DepthImageBuffer,
        LOCAL_ColorHeight * LOCAL_ColorWidth,
        this->ColorCoordinatesInCameraSpace
        );
}

//~~

HRESULT KINECTV2::KinectV2::ProcessMapDepthFrameToCameraSpace(
    void
    )
{
    return this->CoordinateMapper->MapDepthFrameToCameraSpace(
        LOCAL_DepthWidth * LOCAL_DepthHeight,
        this->DepthImageBuffer,
        LOCAL_DepthWidth * LOCAL_DepthHeight,
        this->DepthCoordinatesInCameraSpace
        );
}

//~~

void KINECTV2::KinectV2::ProcessLowDefinitionFaces(
    void
    )
{
    int
        face_index( 0 );
    IFaceFrameResult
        * face_frame_result( nullptr );
    IFaceFrame
        * face_frame( nullptr );
    HRESULT
        result;

    for ( face_index = 0; face_index < BODY_COUNT; ++face_index )
    {
        result = this->LowDefinitionFaceFrameReaders[ face_index ]->AcquireLatestFrame( & face_frame );
        this->LowDefinitionFaceStruct[ face_index ].IsTracked = false;

        if ( SUCCEEDED( result ) && nullptr != face_frame )
        {
            result = face_frame->get_IsTrackingIdValid( &this->LowDefinitionFaceStruct[ face_index ].IsTracked );
        }

        if ( SUCCEEDED( result ) )
        {
            if ( this->LowDefinitionFaceStruct[ face_index ].IsTracked )
            {
                result = face_frame->get_FaceFrameResult( & face_frame_result );

                if ( SUCCEEDED( result ) && face_frame_result != nullptr )
                {
                    result = face_frame_result->get_FaceBoundingBoxInColorSpace( & this->LowDefinitionFaceStruct[ face_index ].BoundingBoxInColorSpace );

                    if ( SUCCEEDED( result ) )
                    {
                        result = face_frame_result->get_FaceBoundingBoxInInfraredSpace( & this->LowDefinitionFaceStruct[ face_index ].BoundingBoxInInfraredSpace );
                    }

                    if ( SUCCEEDED( result ) )
                    {
                        result = face_frame_result->GetFacePointsInColorSpace( FacePointType::FacePointType_Count, this->LowDefinitionFaceStruct[ face_index ].PointsInColorSpace );
                    }

                    if ( SUCCEEDED( result ) )
                    {
                        result = face_frame_result->GetFacePointsInInfraredSpace( FacePointType::FacePointType_Count, this->LowDefinitionFaceStruct[ face_index ].PointsInInfraredSpace );
                    }

                    if ( SUCCEEDED( result ) )
                    {
                        result = face_frame_result->get_FaceRotationQuaternion( & this->LowDefinitionFaceStruct[ face_index ].FaceRotation );
                    }

                    if ( SUCCEEDED( result ) )
                    {
                        result = face_frame_result->GetFaceProperties( FaceProperty::FaceProperty_Count, this->LowDefinitionFaceStruct[ face_index ].Properties );
                    }
                }

                if ( face_frame_result != nullptr )
                {
                    face_frame_result->Release();
                    face_frame_result = nullptr;
                }
            }
            else
            {
                if ( this->BodiesStruct[ face_index ].IsTracked )
                {
                    this->LowDefinitionFaceFrameSources[ face_index ]->put_TrackingId( this->BodiesStruct[ face_index ].TrackingId );
                }
            }
        }

        if ( face_frame != nullptr )
        {
            face_frame->Release();
            face_frame = nullptr;
        }
    }
}

//~~

void KINECTV2::KinectV2::ProcessHighDefinitionFaces(
    void
    )
{
    int
        face_index( 0 );
    IFaceAlignment
        * face_alignment( nullptr );
    IHighDefinitionFaceFrame
        * high_definition_face_frame( nullptr );
    HRESULT
        result;

    for ( face_index = 0; face_index < BODY_COUNT; ++face_index )
    {
        result = this->HighDefinitionFaceFrameReaders[ face_index ]->AcquireLatestFrame( & high_definition_face_frame );
        this->HighDefinitionFaceStruct[ face_index ].IsTracked = false;

        if ( SUCCEEDED( result ) && nullptr != high_definition_face_frame )
        {
            result = high_definition_face_frame->get_IsTrackingIdValid( & this->HighDefinitionFaceStruct[ face_index ].IsTracked );
        }

        if ( SUCCEEDED( result ) && this->HighDefinitionFaceStruct[ face_index ].IsTracked )
        {
            result = CreateFaceAlignment( & face_alignment );
            
            if ( SUCCEEDED( result ) )
            {
                result = high_definition_face_frame->GetAndRefreshFaceAlignmentResult( face_alignment );
            }

            if ( SUCCEEDED( result ) && face_alignment != nullptr )
            {
                result = face_alignment->get_FaceOrientation( & this->HighDefinitionFaceStruct[ face_index ].FaceRotation );
                    
                if ( SUCCEEDED( result ) )
                {
                    result = face_alignment->get_HeadPivotPoint( & this->HighDefinitionFaceStruct[ face_index ].HeadPivotPoint );
                }
            }

            if ( face_alignment != nullptr )
            {
                face_alignment->Release();
                face_alignment = nullptr;
            }
        }
        else
        {
            if ( this->BodiesStruct[ face_index ].IsTracked )
            {
                this->HighDefinitionFaceFrameSources[ face_index ]->put_TrackingId( this->BodiesStruct[ face_index ].TrackingId );
            }
        }

        if ( high_definition_face_frame != nullptr )
        {
            high_definition_face_frame->Release();
            high_definition_face_frame = nullptr;
        }
    }
}
