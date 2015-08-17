#include "mmMuller.h"

using namespace std;
using namespace arma;
using namespace MoMa;

float Muller::meanHumerusLength( Track &track ) {
    
    vec hlPerFrame;
    
    hlPerFrame.resize( track.nOfFrames(), 1 );
    
    if( track.hasNodeList ) {
    
        int LShoulder = track.index( "LShoulder" );
        int RShoulder = track.index( "RShoulder" );
        int LElbow = track.index( "LElbow" );
        int RElbow = track.index( "RElbow" );
        
        for( int t=0; t<track.nOfFrames(); t++ ) {
        
            const mat sliceT = track.position.getData().slice( t );
            
            hlPerFrame( t ) = ( Geometry::distance( sliceT.unsafe_col( LShoulder ), sliceT.unsafe_col( LElbow ) )
            + Geometry::distance( sliceT.unsafe_col( RShoulder ), sliceT.unsafe_col( RElbow ) ) ) / 2.0f;
        }
    }
    
    return( mean( hlPerFrame ) );
}

float Muller::meanShoulderWidth( Track &track ) {
    
    vec swPerFrame;
    
    swPerFrame.resize( track.nOfFrames(), 1 );
    
    if( track.hasNodeList ) {
        
        int LShoulder = track.index( "LShoulder" );
        int RShoulder = track.index( "RShoulder" );
        
        for( int t=0; t<track.nOfFrames(); t++ ) {
        
            const mat sliceT = track.position.getData().slice( t );
            swPerFrame( t ) = Geometry::distance( sliceT.unsafe_col(
            LShoulder ), sliceT.unsafe_col( RShoulder ) );
        }
    }
    
    return( mean( swPerFrame ) );
}

float Muller::meanHipWidth( Track &track ) {
    
    vec hwPerFrame;
    
    hwPerFrame.resize( track.nOfFrames(), 1 );
    
    if( track.hasNodeList ) {
        
        int LHip = track.index( "LHip" );
        int RHip = track.index( "RHip" );
        
        for( int t=0; t<track.nOfFrames(); t++ ) {
            
            const mat sliceT=track.position.getData().slice( t );
            hwPerFrame( t ) = Geometry::distance( sliceT.unsafe_col(
            LHip ), sliceT.unsafe_col( RHip ) );
        }
    }
    
    return( mean( hwPerFrame ) );
}

float Muller::f22( const vec &ar1, const vec &ar2,
const vec &al1, const vec &al2, const vec &lh, const vec &rh ) {
    
    float v23, proj;
    vec al2ar2, vect23, n_hip;
    
    v23 = Geometry::velocityToVect( ar2, al2, al1, al2, ar1, ar2 );
    al2ar2 = normalise(al2-ar2); vect23 = v23*al2ar2;
    
    n_hip = normalise( lh - rh );
    proj = dot( vect23, n_hip );
    
    return( proj );
}

mat Muller::continuous( Track &track ) {

    mat cont;
    
    if( track.nOfFrames() > 0 && track.hasNodeList ) {
        
        cont.set_size( track.nOfFrames(), nOfMullerFeatures );
        cont( 0, 0 ) = 0.0f; // Initialise feature matrix
        
        int Neck = track.index( "Neck" );
        int LShoulder = track.index( "LShoulder" );
        int RShoulder = track.index( "RShoulder" );
        int LElbow = track.index( "LElbow" );
        int RElbow = track.index( "RElbow" );
        int LWrist = track.index( "LWrist" );
        int RWrist = track.index( "RWrist" );
        int Thorax = track.index( "Thorax" );
        int Pelvis = track.index( "Pelvis" );
        int LHip = track.index( "LHip" );
        int RHip = track.index( "RHip" );
        int LKnee = track.index( "LKnee" );
        int RKnee = track.index( "RKnee" );
        int LAnkle = track.index( "LAnkle" );
        int RAnkle = track.index( "RAnkle" );
        int LFoot = track.index( "LFoot" );
        int RFoot = track.index( "RFoot" );
        
        if( track.nOfFrames() > 1 ) {
            
            for( int t=1; t<track.nOfFrames(); t++ ) {
                
                vec nullVec = zeros( 3, 1 );
                vec xVec, yVec, zVec;
                
                xVec << 1 << 0 << 0;
                yVec << 0 << 1 << 0;
                zVec << 0 << 0 << 1;
                
                const mat sliceT_1 = track.position.getData().slice( t-1 );
                const mat sliceT = track.position.getData().slice( t );
                
                // WristRight speed related to Neck in the perpendicular direction to plane ( Neck, HipRight, HipLeft )
                cont( t, 0 ) = Geometry::velocityToNPlane( sliceT_1.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( LHip ), sliceT_1.unsafe_col( RWrist ), sliceT.unsafe_col( RWrist ) ) * track.frameRate();
                
                // WristLeft speed related to Neck in the perpendicular direction to plane ( Neck, HipRight, HipLeft )
                cont( t, 1 ) =  Geometry::velocityToNPlane( sliceT_1.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( LHip ), sliceT_1.unsafe_col( LWrist ), sliceT.unsafe_col( LWrist ) ) * track.frameRate();
                
                // Distance between WristRight and plane fixed in Neck and normal to vector Thorax-> Neck
                cont( t, 2 ) = Geometry::distanceToNPlane( sliceT.unsafe_col( Thorax ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( RWrist ) );
                
                // Distance between WristLeft and plane fixed in Neck and normal to vector Thorax-> Neck
                cont( t, 3 ) = Geometry::distanceToNPlane( sliceT.unsafe_col( Thorax ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( LWrist ) );
                
                // WristRight speed related to SpineMiddle in the direction SpineBase->SpineMiddle : TODO change back neck to thorax with updated thorax
                cont( t, 4 ) = Geometry::velocityToVect( sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( Neck ), sliceT_1.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT_1.unsafe_col( RWrist ), sliceT.unsafe_col( RWrist ) ) * track.frameRate();
                
                // WristLeft speed related to SpineMiddle in the direction SpineBase->SpineMiddle
                cont( t, 5 ) = Geometry::velocityToVect( sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( Neck ), sliceT_1.unsafe_col( Neck ), sliceT.unsafe_col( Neck ), sliceT_1.unsafe_col( LWrist ), sliceT.unsafe_col( LWrist ) ) * track.frameRate();
                
                // Angle between ElbowRight->ShoulderRight and ElbowRight->WristRight vectors
                cont( t, 6 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( RElbow ), sliceT.unsafe_col( RShoulder ), sliceT.unsafe_col( RElbow ), sliceT.unsafe_col( RWrist ) ) );
                
                // Angle between ElbowLeft->ShoulderLeft and ElbowLeft->WristLeft vectors
                cont( t, 7 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( LElbow ), sliceT.unsafe_col( LShoulder ), sliceT.unsafe_col( LElbow ), sliceT.unsafe_col( LWrist ) ) );
                
                // Distance between WristLeft and plane in WristRight and normal to ShoulderLeft->ShoulderRight
                cont( t, 8 ) = Geometry::distanceToNPlane( sliceT.unsafe_col( LShoulder ), sliceT.unsafe_col( RShoulder ), sliceT.unsafe_col( LWrist ), sliceT.unsafe_col( RWrist ) );
                
                // WristLeft speed related to WristRight in the direction WristLeft->WristRight
                cont( t, 9 ) = Geometry::velocityToVect( sliceT.unsafe_col( LWrist ), sliceT.unsafe_col( RWrist ), sliceT_1.unsafe_col( RWrist ), sliceT.unsafe_col( RWrist ), sliceT_1.unsafe_col( LWrist ), sliceT.unsafe_col( LWrist ) ) * track.frameRate();
                
                // WristRight speed related to SpineBase in the direction of WristRight->SpineBase
                cont( t, 10 ) = Geometry::velocityToVect( sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( RWrist ), sliceT_1.unsafe_col( Pelvis ), sliceT.unsafe_col( Pelvis ), sliceT_1.unsafe_col( RWrist ), sliceT.unsafe_col( RWrist ) ) * track.frameRate();
                
                // WristLeft speed related to SpineBase in the direction of WristLeft->SpineBase
                cont( t, 11 ) = Geometry::velocityToVect( sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( LWrist ), sliceT_1.unsafe_col( Pelvis ), sliceT.unsafe_col( Pelvis ), sliceT_1.unsafe_col( LWrist ), sliceT.unsafe_col( LWrist ) ) * track.frameRate();
                
                // WristRight speed
                cont( t, 12 ) = norm( sliceT_1.unsafe_col( RWrist ) - sliceT.unsafe_col( RWrist ) ) * track.frameRate();
                
                // WristLeft speed
                cont( t, 13 ) = norm( sliceT_1.unsafe_col( LWrist ) - sliceT.unsafe_col( LWrist ) ) * track.frameRate();
                
                // Distance between AnkleRight and plane defined by ( SpineBase, HipLeft, FootLeft )
                cont( t, 14 ) = - Geometry::distanceToPlane( sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( LFoot ), sliceT.unsafe_col( RAnkle ) );
                
                // Distance between AnkleLeft and plane defined by ( SpineBase, HipRight, FootRight )
                cont( t, 15 ) = - Geometry::distanceToPlane( sliceT.unsafe_col( RFoot ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( LAnkle ) );
                
                // Distance between AnkleRight and plane parallel to ground and fixed at the minimum height of all body points - AnkleRight
                cont( t, 16 ) = Geometry::distanceToNPlane( nullVec, zVec, zVec*BoundingBox::lowest( sliceT,  RAnkle  )( Z ), sliceT.unsafe_col( RAnkle ) );
                
                // Distance between AnkleLeft and plane parallel to ground and fixed at the minimum height of all body points - AnkleLeft
                cont( t, 17 ) = Geometry::distanceToNPlane( nullVec, zVec, zVec*BoundingBox::lowest( sliceT,  LAnkle  )( Z ), sliceT.unsafe_col( LAnkle ) );
                
                // Distance between AnkleRIght and plane fixed in AnkleLeft and normal to vector HipLeft->HipRight
                cont( t, 18 ) = Geometry::distanceToNPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( LAnkle ), sliceT.unsafe_col( RAnkle ) );
                
                // Angle between KneeRight->HipRight and KneeRight->AnkleRight vectors
                cont( t, 19 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( RKnee ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( RKnee ), sliceT.unsafe_col( RAnkle ) ) );
                
                // Angle between KneeLeft->HipLeft and KneeLeft->Ankleft vectors
                cont( t, 20 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( LKnee ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( LKnee ), sliceT.unsafe_col( LAnkle ) ) );
                
                // Is distance between AnkleRight and plane fixed in HipLeft and normal to vector HipLeft->HipRight smaller than distance between AnkleLeft and the same plan (feet crossed over)
                cont( t, 21 ) = - Geometry::distanceToNPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RAnkle ) ) + Geometry::distanceToNPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( LAnkle ) ); // threshold=0
                
                // AnkleRight speed related to AnkleLeft in the direction AnkleRight->AnkleLeft projected on direction HipRight->HipLeft (lateral cross-feet speed)
                cont( t, 22 ) = f22( sliceT_1.unsafe_col(RAnkle ), sliceT.unsafe_col( RAnkle ), sliceT_1.unsafe_col( LAnkle ), sliceT.unsafe_col( LAnkle ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ) )* track.frameRate();
                
                // Inverse of muller (t, 22)
                cont( t, 23 ) = -cont( t, 22 );
                
                // AnkleRight speed
                cont( t, 24 ) = norm( sliceT_1.unsafe_col( RAnkle ) - sliceT.unsafe_col( RAnkle ) )* track.frameRate();
                
                // AnkleLeft speed
                cont( t, 25 ) = norm( sliceT_1.unsafe_col( LAnkle ) - sliceT.unsafe_col( LAnkle ) )* track.frameRate();
                
                // Angle between Neck->SpineBase and ShoulderRight->ElbowRight vectors (angle between right arm and body spine)
                cont( t, 26 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( RShoulder ), sliceT.unsafe_col( RElbow ) ) );
                
                // Angle between Neck->SpineBase and ShoulderLeft->ElbowLeft vectors (angle between left arm and body spine)
                cont( t, 27 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( LShoulder ), sliceT.unsafe_col( LElbow ) ) );
                
                // Angle between Neck->SpineBase and HipRight->KneeRight vectors (angle between right leg and body spine)
                cont( t, 28 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( RKnee ) ) );
                
                // Angle between Neck->SpineBase and HipLeft->KneeLeft vectors (angle between left leg and body spine)
                cont( t, 29 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Pelvis ), sliceT.unsafe_col( LHip ), sliceT.unsafe_col( LKnee ) ) );
                
                // Distance between SpineBase and plane defined by ( AnkleRight, Neck, AnkleLeft )
                cont( t, 30 ) = - Geometry::distanceToPlane( sliceT.unsafe_col( RAnkle ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( LAnkle ), sliceT.unsafe_col( Pelvis ) );
                
                // Angle between vector Neck->SpineBase and vector perpendicular to ground (verticalness of body spine)
                cont( t, 31 ) = Geometry::radToDeg( Geometry::angleBtwVectors( sliceT.unsafe_col( Neck ), sliceT.unsafe_col( Pelvis ), nullVec, zVec ) );
                
                // Distance between WristRight and plan parallel to ground (normal inside ground) and fixed at minimum height of body joints - WristRight
                cont( t, 32 ) = Geometry::distanceToNPlane( nullVec, -1*zVec,  zVec * BoundingBox::lowest( sliceT,  RWrist  )( Z ), sliceT.unsafe_col( RWrist ) );
                
                // Distance between WristLeft and plan parallel to ground (normal inside ground) and fixed at minimum height of body joints - WristLeft
                cont( t, 33 ) = Geometry::distanceToNPlane( nullVec, -1*zVec ,zVec * BoundingBox::lowest( sliceT,  LWrist  )( Z ), sliceT.unsafe_col( LWrist ) );
                
                // Is distance between ShoulderRight and plane ( HipRight, HipLeft, Neck ) smaller than distance between ShoulderLeft and the same plan
                cont( t, 34 ) =  -Geometry::distanceToPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( RShoulder ) ) + Geometry::distanceToPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( LShoulder ) ); //- Geometry::distanceToPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( LShoulder ) );
                
                // Symmetrical value of F[34]: is it useful (yes if threshold adapted! (feature itself not useful (=-muller(t,34), but binary feature is)?
                cont( t, 35 ) = -cont( t , 34 ); // Geometry::distanceToPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( LShoulder ) ) - Geometry::distanceToPlane( sliceT.unsafe_col( LHip ), sliceT.unsafe_col( RHip ), sliceT.unsafe_col( Neck ), sliceT.unsafe_col( RShoulder ) );
                
                // Distance between lowest and highest points of the body
                cont( t, 36 ) = norm( BoundingBox::highest( sliceT )- BoundingBox::lowest( sliceT ) );
                
                // Biggest distance of the projected body on the ground
                cont( t, 37 ) = BoundingBox::width( sliceT );
                
                // SpineBase speed
                cont( t, 38 ) = norm( sliceT_1.unsafe_col(Pelvis ) - sliceT.unsafe_col( Pelvis ) )* track.frameRate();
            }
        }
    }
    
    return( cont );
}

mat Muller::binary( mat &cont, float meanHl, float meanSw, float meanHw ) {

    mat bina;
    
    bina.set_size( cont.n_rows, cont.n_cols );
    
    if( bina.n_rows > 0 ) {
        
        bina( 0, 0 ) = 0.0f; // Initialise matrix
        
        bina.col( 0 )= Signal::thresh( cont.col( 0 ), 1.8 * meanHl, 1.3 * meanHl, 0, 1 );
        bina.col( 1 ) = Signal::thresh( cont.col( 1 ), 1.8 * meanHl, 1.3 * meanHl, 0, 1 );
        bina.col( 2 ) = Signal::thresh( cont.col( 2 ), 0.2 * meanHl, 0, 0, 1 );
        bina.col( 3 ) = Signal::thresh( cont.col( 3 ), 0.2 * meanHl, 0, 0, 1 );
        bina.col( 4 ) = Signal::thresh( cont.col( 4 ), 1.8 * meanHl, 1.3 * meanHl, 0, 1 );
        bina.col( 5 ) = Signal::thresh( cont.col( 5 ), 1.8 * meanHl, 1.3 * meanHl, 0, 1 );
        bina.col( 6 ) = Signal::thresh( -cont.col( 6 ), -110, -120, 0, 1 );
        bina.col( 7 ) = Signal::thresh( -cont.col( 7 ), -110, -120, 0, 1 );
        bina.col( 8 ) = Signal::thresh( cont.col( 8 ), 2.5 * meanSw, 2.0 * meanSw, 0, 1 );
        bina.col( 9 ) = Signal::thresh( cont.col( 9 ), 1.4 * meanSw, 1.2 * meanSw, 0, 1 );
        bina.col( 10 ) = Signal::thresh( cont.col( 10 ), 1.4 * meanSw, 1.2 * meanSw, 0, 1 );
        bina.col( 11 ) = Signal::thresh( cont.col( 11 ), 1.4 * meanSw, 1.2 * meanSw, 0, 1 );
        bina.col( 12 ) = Signal::thresh( cont.col( 12 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 13 ) = Signal::thresh( cont.col( 13 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 14 ) = Signal::thresh( cont.col( 14 ), 0.38 * meanHl, 0, 0, 1 );
        bina.col( 15 ) = Signal::thresh( cont.col( 15 ), 0.38 * meanHl, 0, 0, 1 );
        bina.col( 16 ) = Signal::thresh( cont.col( 16 ), 1.2 * meanHl, 1 * meanHl, 0, 1 );
        bina.col( 17 ) = Signal::thresh( cont.col( 17 ), 1.2 * meanHl, 1 * meanHl, 0, 1 );
        bina.col( 18 ) = Signal::thresh( cont.col( 18 ), 2.1 * meanHw, 1.8 * meanHw, 0, 1 );
        bina.col( 19 ) = Signal::thresh( -cont.col( 19 ), -110, -120, 0, 1 );
        bina.col( 20 ) = Signal::thresh( -cont.col( 20 ), -110, -120, 0, 1 );
        bina.col( 21 ) = Signal::thresh( cont.col( 21 ), 3, 0, 0, 1 );
        bina.col( 22 ) = Signal::thresh( cont.col( 22 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 23 ) = Signal::thresh( cont.col( 23 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 24 ) = Signal::thresh( cont.col( 24 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 25 ) = Signal::thresh( cont.col( 25 ), 2.5 * meanHl, 2.0 * meanHl, 0, 1 );
        bina.col( 26 ) = Signal::thresh( cont.col( 26 ), 25, 20, 0, 1 );
        bina.col( 27 ) = Signal::thresh( cont.col( 27 ), 25, 20, 0, 1 );
        bina.col( 28 ) = Signal::thresh( cont.col( 28 ), 50, 40, 0, 1 );
        bina.col( 29 ) = Signal::thresh( cont.col( 29 ), 50, 40, 0, 1 );
        bina.col( 30 ) = Signal::thresh( cont.col( 30 ), 0.5 * meanHl, 0.35 * meanHl, 0, 1 );
        bina.col( 31 ) = Signal::thresh( cont.col( 31 ), 70, 60, 0, 1 );
        bina.col( 32 ) = Signal::thresh( cont.col( 32 ), -1.1 * meanHl, -1.3 * meanHl, 0, 1 );
        bina.col( 33 ) = Signal::thresh( cont.col( 33 ), -1.1 * meanHl, -1.3 * meanHl, 0, 1 );
        bina.col( 34 ) = Signal::thresh( cont.col( 34 ), 100, 90, 0, 1 );
        bina.col( 35 ) = Signal::thresh( cont.col( 35 ), 100, 90, 0, 1 );
        bina.col( 36 ) = Signal::thresh( -cont.col( 36 ), -4 * meanHl, -4.3 * meanHl, 0, 1 );
        bina.col( 37 ) = Signal::thresh( cont.col( 37 ), 4 * meanHl, 3.8 * meanHl, 0, 1 );
        bina.col( 38 ) = Signal::thresh( cont.col( 38 ), 2.3 * meanHl, 1.9 * meanHl, 0, 1 );
    }
    
    return( bina );
}

mat Muller::rescale( mat &cont, mat &bina ) {

    mat rescaled;
    
    rescaled.set_size( bina.n_rows, bina.n_cols );
    
    if( rescaled.n_rows > 0 ) {
        
        rescaled( 0, 0 ) = 0.0f; // Initialise matrix
    
        for( int c=0; c<rescaled.n_cols; c++ ) {
        
            rescaled.col( c ) = bina.col( c ) * Signal::max( cont.col( c ) );
        }
    }
    
    return( rescaled );
}

arma::mat Muller::binary( Track &track, int type ) {

    mat result;
    
    mat cont = continuous( track );
    
    float meanHl = meanHumerusLength( track );
    float meanSw = meanShoulderWidth( track );
    float meanHw = meanHipWidth( track );
    
    if( type == BINARY ) {
    
        result = binary( cont, meanHl, meanSw, meanHw );
        
    } else if( type == RESCALE ) {
    
        mat bina = binary( cont, meanHl, meanSw, meanHw );
        result = rescale( cont, bina ); // Apply rescale
    }
    
    return( result );
}

MoMa::Muller::ThresholdNames::ThresholdNames( void ) {
    
    this->resize( nOfMullerFeatures );
    
    this->at( 0 ) = "Right hand moves forwards";
    this->at( 1 ) = "Left hand moves forwards";
    this->at( 2 ) = "Right hand is above neck";
    this->at( 3 ) = "Left hand is above neck";
    this->at( 4 ) = "Right hand moves upwards";
    this->at( 5 ) = "Left hand moves upwards";
    this->at( 6 ) = "Right elbow is bent";
    this->at( 7 ) = "Left elbow is bent";
    this->at( 8 ) = "Hands are far apart sideways";
    this->at( 9 ) = "Hands approach each other";
    this->at( 10 ) = "Right hand moves away from root";
    this->at( 11 ) = "Left hand moves away from root";
    this->at( 12 ) = "Right hand is fast";
    this->at( 13 ) = "Left hand is fast";
    this->at( 14 ) = "Right foot is behind leg";
    this->at( 15 ) = "Left foot is behind leg";
    this->at( 16 ) = "Right foot is raised";
    this->at( 17 ) = "Left foot is raised";
    this->at( 18 ) = "Feet are far apart sideways";
    this->at( 19 ) = "Right knee is bent";
    this->at( 20 ) = "Left knee is bent";
    this->at( 21 ) = "Feet crossed over";
    this->at( 22 ) = "Feet move towards each other sideways";
    this->at( 23 ) = "Feet move apart sideways";
    this->at( 24 ) = "Right foot is fast";
    this->at( 25 ) = "Left foot is fast";
    this->at( 26 ) = "Right humerus is abducted";
    this->at( 27 ) = "Left humerus is abducted";
    this->at( 28 ) = "Right femur is abducted";
    this->at( 29 ) = "Left femur is abducted";
    this->at( 30 ) = "Root is behind frontal plane";
    this->at( 31 ) = "Spine is horizontal";
    this->at( 32 ) = "Right hand is lowered";
    this->at( 33 ) = "Left hand is lowered";
    this->at( 34 ) = "Shoulders are rotated right";
    this->at( 35 ) = "Shoulders are rotated left";
    this->at( 36 ) = "Y-extents of body is small";
    this->at( 37 ) = "XZ-extents of body is large";
    this->at( 38 ) = "Root is fast";
}
