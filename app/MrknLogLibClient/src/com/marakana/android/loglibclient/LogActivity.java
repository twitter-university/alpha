package com.marakana.android.loglibclient;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import com.marakana.android.lib.log.LibLog;

public class LogActivity extends Activity 
  implements View.OnClickListener, Runnable {

  private TextView output;
  private Handler handler;
  private LibLog libLog;

  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    super.setContentView(R.layout.log);
    this.output = (TextView) super.findViewById(R.id.output);
    Button button = (Button) super.findViewById(R.id.button);
    button.setOnClickListener(this);
    this.handler = new Handler();
  }

  @Override
  public void onResume() {
    super.onResume();
    this.libLog = new LibLog();
    this.handler.post(this);
  }
 
  @Override
  public void onPause() {
    super.onPause();
    this.handler.removeCallbacks(this);
    this.libLog.close();
  }

  public void onClick(View view) {
    this.libLog.flushLog();
    this.updateOutput();
  }

  public void run() {
    this.updateOutput();
    this.handler.postDelayed(this, 1000);
  }

  private void updateOutput() {
    int usedLogSize = this.libLog.getUsedLogSize();
    int totalLogSize = this.libLog.getTotalLogSize();
    this.output.setText(
      super.getString(R.string.log_utilization_message,      
      usedLogSize, totalLogSize));
  }
}
