#ifndef __itkSLICSuperVoxelImageFilter_h_
#define __itkSLICSuperVoxelImageFilter_h_

#include "itkImageToImageFilter.h"
#include "itkSmartPointer.h"
#include "itkBarrier.h"

namespace itk {

template <typename TInputImage, typename TLabelImage, typename TRealImage>
class ITK_EXPORT SLICSuperVoxelImageFilter : public ImageToImageFilter<TInputImage, TLabelImage> 
{
public:
  typedef SLICSuperVoxelImageFilter Self;
  typedef ImageToImageFilter<TInputImage, TLabelImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  typedef typename Superclass::InputImageType  InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename InputImageType::PixelType   InputPixelType;
  typedef typename OutputImageType::PixelType  OutputPixelType;
  typedef typename OutputImageType::RegionType RegionType;
  typedef typename InputImageType::IndexType IndexType;
  typedef typename InputImageType::SizeType SizeType;
  typedef typename InputImageType::OffsetType OffsetType;
  typedef typename InputImageType::PointType PointType;
  typedef typename TRealImage::PixelType RealPixelType;

  /** Image dimension */
  itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SLICSuperVoxelImageFilter, ImageToImageFilter);

  /** Set a scalar-valued gradient magnitude image, used to push initial cluster
   *  centers away from the edges */
  void SetGradientImage(TRealImage *gradImage);

  /** Set the number of supervoxel seeds in each dimension */
  itkSetMacro(SeedsPerDimension, int);
  itkGetConstMacro(SeedsPerDimension, int);

  /** Set the weighting between intensity similarity and spatial distance */
  itkSetMacro(MParameter, double);
  itkGetConstMacro(MParameter, double);

protected:

  SLICSuperVoxelImageFilter();
  ~SLICSuperVoxelImageFilter() {}

  void PrintSelf(std::ostream & os, Indent indent) const override {};

  void BeforeThreadedGenerateData(void) override;

  void ThreadedGenerateData(const RegionType & outputRegionForThread, ThreadIdType threadId) override;

private:
  SLICSuperVoxelImageFilter(const Self &); //purposely not
                                                       // implemented
  void operator=(const Self &);                        //purposely not
                                                       // implemented

  typename TRealImage::Pointer m_DistanceImage;
  typename TRealImage::Pointer m_GradientImage;
  double m_MParameter;
  double m_SeedsPerDimension;

  struct Cluster {
    IndexType Index;
    InputPixelType Pixel;
    long Count;
    InputPixelType MinPixel, MaxPixel;
    double MaxPixelDist;
    void Reset() 
      { Index.Fill(0); Pixel = 0; Count = 0; 
        MinPixel = 1e100; MaxPixel = -1e100; 
        MaxPixelDist = 0; }
  };

  // Cluster center descriptor
  typedef std::vector<Cluster> ClusterVector;
  ClusterVector m_Clusters;

  // Per-thread cluster lists
  std::vector<ClusterVector> m_PerThreadClusters;

  // Barrier for threading 
  typename Barrier::Pointer m_Barrier;

  // Search region size
  SizeType m_SearchRegionSize;
  OffsetType m_SearchRegionOffset;
  double m_SearchRegionMaxWidth;

  // Distance computation
  double ComputeDistance(const IndexType &index, const InputPixelType &pixel, Cluster &cluster);
};

// namespace
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSLICSuperVoxelImageFilter.hxx"
#endif

#endif
