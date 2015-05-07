#include "wavepreview.h"
#include "globals.h"
#include <QPushButton>


namespace MusEGlobal
{
MusECore::WavePreview *wavePreview;
}

namespace MusECore
{

WavePreview::WavePreview():
   sf(0),
   isPlaying(false),
   src(0)
{
   segSize = MusEGlobal::segmentSize * 10;
   tmpbuffer = new float [segSize];
   srcbuffer = new float [segSize];
}

WavePreview::~WavePreview()
{
   stop();
   delete tmpbuffer;
   delete srcbuffer;
}

void WavePreview::play(QString path)
{
   stop();
   memset(&sfi, 0, sizeof(sfi));   
   sf = sf_open(path.toUtf8().data(), SFM_READ, &sfi);
   if(sf)
   {
      int err = 0;
      src = src_new(SRC_SINC_BEST_QUALITY, sfi.channels, &err);
      if(src)
      {
         p1 = tmpbuffer;
         p2 = srcbuffer;      
         f1 = 0;
         f2 = 0;
         sd.src_ratio = ((double)MusEGlobal::sampleRate) / (double)sfi.samplerate;
         isPlaying = true;      
      }
      else
      {
         sf_close(sf);
         sf = 0;
      }

   }

}

void WavePreview::stop()
{
   isPlaying = false;
   if(sf)
   {
      sf_close(sf);
      sf = 0;
   }
   if(src)
   {
      src_delete(src);
      src = 0;           
   }
}

void WavePreview::addData(int channels, int nframes, float *buffer[])
{
   if(sf && isPlaying)
   {
      sf_count_t nread = sf_readf_float(sf, tmpbuffer, nframes);
      if(nread <= 0)
      {
         isPlaying = false;
         return;
      }
      memset(srcbuffer, 0, sizeof(segSize) * sizeof(float));
      p2 = 0;
      f2 = 0;      
      
      while(true)
      {
         if(f1 >= nframes)
         {
            f1 = 0;
            p1 = tmpbuffer;
            sf_count_t nread = sf_readf_float(sf, tmpbuffer, nframes);

            if(nread <= 0)
            {
               isPlaying = false;
               return;
            }
         }
         sd.data_in = p1;
         sd.data_out = p2;
         sd.end_of_input = (nread == nframes) ? false : true;
         sd.input_frames = nread;
         sd.output_frames = nframes - f2;
         sd.input_frames_used = sd.output_frames_gen = 0;
         int err = src_process(src, &sd);
         if(err != 0)
         {
            break;
         }
         p1 += sd.input_frames_used * sfi.channels * sizeof(float);
         p2 += sd.output_frames_gen * sfi.channels * sizeof(float);
         f1 += sd.input_frames_used;
         f2 += sd.output_frames_gen;
         
      }
      
      int chns = std::min(channels, sfi.channels);
      for(int i = 0; i < chns; i++)
      {
         for(int k = 0; k < nread; k++)
         {
            buffer [i] [k] += tmpbuffer [(k + i)*sfi.channels];
         }
      }
      for(int i = chns; i < channels; i++)
      {
         for(int k = 0; k < nread; k++)
         {
            buffer [i] [k] += tmpbuffer [(k + i)*sfi.channels];
         }
      }
   }
}

void initWavePreview()
{
   MusEGlobal::wavePreview = new WavePreview();
}

void exitWavePreview()
{
   if(MusEGlobal::wavePreview)
   {
      delete MusEGlobal::wavePreview;
   }
}

void AudioPreviewDialog::urlChanged(const QString &str)
{
   QFileInfo fi(str);
   if(fi.isDir()){
      return;
   }
   MusEGlobal::wavePreview->play(str);
}

void AudioPreviewDialog::stopWave()
{
   MusEGlobal::wavePreview->stop();
}

AudioPreviewDialog::AudioPreviewDialog(QWidget *parent)
   :QFileDialog(parent)
{
    setOption(QFileDialog::DontUseNativeDialog);
    setNameFilter(QString("Samples *.wav *.ogg *.flac (*.wav *.WAV *.ogg *.flac);;All files (*)"));
    cb = new QComboBox;
    cb->setEditable(false);
    //cb->addItems(list_ports());
    cb->setCurrentIndex(cb->count() - 1);

    QPushButton *btnStop = new QPushButton("Stop");
    connect(btnStop, SIGNAL(clicked()), this, SLOT(stopWave()));


    //QListView *v = this->findChild<QListView *>("listView", Qt::FindChildrenRecursively);
    QObject::connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(urlChanged(const QString&)));
    //this->layout()->addWidget(new QLabel("Midi device: "));
    //this->layout()->addWidget(cb);
    //this->layout()->addWidget(btnStop);
}

AudioPreviewDialog::~AudioPreviewDialog()
{
   MusEGlobal::wavePreview->stop();
}

}
