/*!*****************************************************************
@file
    H.265  software codec
    reference document ITU-T H.265 (04/2013)
    **H.264 stream information**
    developed by mojo.
@author
    Copyright (c)2002-2015 mojo,all rights reserved
*******************************************************************/
#ifndef _H265StreamInfo_h
#define _H265StreamInfo_h


#include "ColorSpaceType.h"

#define MAXCODEC	16

#ifndef MaxDecPicNum
	#define MaxDecPicNum 16
#endif

//!  M4M3 decoder parameter.
typedef struct _M4M3DecPar
{
	int m_ThreadNum,				//!< decoder thread
		m_CPUFlag,					//!< CPU SIMD flag,auto=0,GenCPU=1,SSE2CPU=(1<<3),AVXCPU=(1<<6),AVX2CPU=(1<<7)
		m_AutoSkipDeblockingFlag;	//!< auto skip deblocking flag
}M4M3DecPar;


//!  H265 stream information.
typedef struct _H265StreamInfo
{
	int m_profile_idc,			//!<stream profile
		m_tier_flag,				//<!stream tier
		m_level_idc,			//!<stream level
		m_Progressive,			//!<1:Progressive,0: interlace
		m_FrameWidth,			//!<video frame Width
		m_FrameHeight,			//!<video frame Height
		m_FrameCropWidth,			//!<video frame crop Width
		m_FrameCropHeight;			//!<video frame crop Height
	ColorSpaceType m_ColorSpace;			//!<video color space
}H265StreamInfo;

//!  M4M3 decoder information.
typedef struct _M4M3DecoderInfo
{
	char m_SupportColorSpaceConvert[TotalColorSpace][TotalColorSpace];	//!<supported color space convert Index=[SrcSpace][DesSpace],>0:support ,0:not support
	char m_bSupportProfile[8];											//!<supported profile, index=[0:baseline,1:main,2:extend,3:high,4:high10,5:high422,6:high444],>0:support,0:not support
	char m_Ver[64];														//!<decoder version
	int	m_MaxThread;													//!<decoder support max thread
}M4M3DecoderInfo;

typedef M4M3DecoderInfo M4M3EncoderInfo;


/////////////////encoder
//!  M4M3 encoder parameter.
typedef struct _ScalingMatrix
{
	unsigned short	m_4x4ScalingMatrix[6][16],
					m_8x8ScalingMatrix[6][64],
					m_16x16ScalingMatrix[6][65],//m_p16x16ScalingMatrix[][64]=DC,
					m_32x32ScalingMatrix[2][65];//m_p32x32ScalingMatrix[][64]=DC
}SScalingMatrix;
typedef struct _ShortTermRefPicSet
{
	char			m_inter_ref_pic_set_prediction_flag,
					m_delta_idx;
	unsigned char	m_NumDeltaPocs,
					m_NumNegativePics,
					m_NumPositivePics;
	short			m_DeltaRPS;
	int				m_DeltaPocS[2][MaxDecPicNum];		//[2][MaxDecPicNum]

	unsigned int	m_UsedByCurrPicFlag,		/// bit field flag
					m_UseDeltaFlag;				/// bit field flag,

}SShortTermRefPicSet;
typedef struct _M4M3EncPar
{
//VideoParameterSet
	char			m_vps_max_layers,
					m_vps_max_sub_layers,
					m_vps_temporal_id_nesting_flag;
//ProfileTierLevel
	char			m_general_profile_space,//0
					m_general_tier_flag,	//0:main tier,1:high tier
					m_general_profile_idc,	//main profile: 1,main10 profile:2,main still:3
					m_general_progressive_source_flag,
					m_general_interlaced_source_flag,
					m_general_non_packed_constraint_flag,
					m_general_frame_only_constraint_flag,
					m_general_level_idc;	//level*30

	int				m_general_profile_compatibility_flag;	//main profile  : 2
															//main10 profile: 6
															//main still    : 8

//SeqParameterSet
	unsigned char	m_sps_max_sub_layers,
					m_sps_temporal_id_nesting_flag,
					m_chroma_format_idc,
					m_separate_colour_plane_flag,
					m_conformance_window_flag,
					m_BitDepthYCbr[2],
					m_PCMBitDepthY[2],
					m_Log2_max_pic_order_cnt_lsb,
					m_Log2MinCbSizeY,
					m_Log2CtbSizeY,
					m_Log2MinTrafoSize,
					m_Log2MaxTrafoSize,
					m_max_transform_hierarchy_depth_inter,
					m_max_transform_hierarchy_depth_intra,
					m_scaling_list_enable_flag,
					m_sps_scaling_list_data_present_flag,
					m_CustomScalingMatrix,//0:Low pass,1:Low bit,2:animation
					m_amp_enabled_flag,
					m_sample_adaptive_offset_enabled_flag,
					m_pcm_enabled_flag,
					m_Log2MinIpcmCbSizeY,
					m_Log2MaxIpcmCbSizeY,
					m_pcm_loop_filter_disabled_flag,
					m_num_short_term_ref_pic_sets,
					m_long_term_ref_pics_present_flag,
					m_num_long_term_ref_pics_sps,
					m_sps_temporal_mvp_enabled_flag,
					m_strong_intra_smoothing_enabled_flag,
					m_vui_parameters_present_flag;

	unsigned short	m_pic_width_in_luma_samples,
					m_pic_height_in_luma_samples,
					m_conf_win_left_offset,
					m_conf_win_right_offset,
					m_conf_win_top_offset,
					m_conf_win_bottom_offset,
			//		m_sps_max_num_reorder_pics[MAX_SUBLAYER],
					m_PicWidthInMinCbsY,
					m_PicWidthInCtbsY,
					m_PicHeightInMinCbsY,
					m_PicHeightInCtbsY;
					

	int				m_PicSizeInMinCbsY,
					m_PicSizeInCtbsY;
				//	m_PicSizeInSamplesY;
	unsigned int	m_lt_ref_pic_poc_lsb_sps[MaxDecPicNum],
					m_used_by_curr_pic_lt_sps_flag;
//	SScalingMatrix	m_SeqScalingMatrix;
	SShortTermRefPicSet	m_SeqShortTermRefPicSet[MaxDecPicNum];

//VUI


//PicParameterSet
	unsigned char	m_dependent_slice_segments_enabled_flag,
					m_output_flag_present_flag,
					m_num_extra_slice_header_bits,
					m_sign_data_hiding_enabled_flag,
					m_cabac_init_present_flag,
					m_num_ref_idx_lx_default_active[2],
					m_init_qp,
					m_constrained_intra_pred_flag,
					m_transform_skip_enabled_flag,
					m_cu_qp_delta_enabled_flag,
					m_Log2MinCuQpDeltaSize,
					m_pps_slice_chroma_qp_offsets_present_flag,
					m_weighted_pred_flag,
					m_weighted_bipred_flag,
					m_transquant_bypass_enabled_flag,
					m_tiles_enabled_flag,
					m_entropy_coding_sync_enabled_flag,
					m_uniform_spacing_flag,
					m_loop_filter_across_tiles_enabled_flag,
					m_pps_loop_filter_across_slices_enabled_flag,
					m_deblocking_filter_control_present_flag,
					m_deblocking_filter_override_enabled_flag,
					m_pps_deblocking_filter_disabled_flag,
					m_pps_scaling_list_data_present_flag,
					m_lists_modification_present_flag,
					m_log2_parallel_merge_level,
					m_slice_segment_header_extension_present_flag
					;
	char			m_pps_cb_qp_offset,
					m_pps_cr_qp_offset,
					m_pps_beta_offset,
					m_pps_tc_offset;
	unsigned short	m_num_tile_columns,
					m_num_tile_rows
					;
	SScalingMatrix	m_PicScalingMatrix;
//slice
	char			m_slice_sao_luma_flag,
					m_slice_sao_chroma_flag;
//rate control
	float			m_FrameRate; 
	int				m_RateControl,
					m_CBBitrate,
					m_VBRQuality,
					m_GOP_Type,
					m_IDR_Interval;
//prediction

	unsigned char	//m_PAFF,										//use picture adaptive frame/field ot not
					m_bSAtD;
	int				m_PredMethod,								//1:QUALITY,2:NORMAL,4:SPEED
					m_log2Range;								//!motion estimation range
////source video
	bool			m_bDeinterlace,
					m_SrcVideoVFlip;							//!<source video flip or not,for BGR only
	char			m_SrcVideoColorSpace;
	//				m_src_chroma_format_idc,
	//				m_SrcBitDepthY,								//!<source video bit depth of the samples of the luma, 
	//				m_SrcBitDepthC;								//!<source video bit depth of the samples of the chroma,

	short			m_SrcVideoWidth,							//!<source video width
					m_SrcVideoHeight;							//!<source video height

//CPU
	int				m_CPUFlag,					//!< CPU SIMD flag,auto=0,GenCPU=1,SSE2CPU=(1<<3),AVXCPU=(1<<6),AVX2CPU=(1<<7)
					m_ProcessNum;
}M4M3EncPar;

#endif
