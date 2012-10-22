
package com.marakana.android.mrknlogclient;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.marakana.android.service.log.LogListener;
import com.marakana.android.service.log.LogManager;

public class LogActivity extends Activity implements OnClickListener, LogListener {

  private TextView output;

  private final LogManager logManager = LogManager.getInstance();

  private final int totalLogSize = this.logManager.getTotalLogSize();

  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    super.setContentView(R.layout.log);
    this.output = (TextView)super.findViewById(R.id.output);
    Button button = (Button)super.findViewById(R.id.button);
    button.setOnClickListener(this);
  }

  private void updateOutput(int usedLogSize) {
    this.output.setText(super.getString(R.string.log_utilization_message, 
      usedLogSize, this.totalLogSize));
  }

  @Override
  public void onResume() {
    super.onResume();
    this.logManager.register(this);
    this.updateOutput(this.logManager.getUsedLogSize());
  }

  @Override
  public void onPause() {
    super.onPause();
    this.logManager.unregister(this);
  }

  @Override
  public void onClick(View view) {
    this.logManager.flushLog();
    this.updateOutput(0);
  }
  
  @Override
  public void onUsedLogSizeChange(int usedLogSize) {
    this.updateOutput(usedLogSize);
  }
}
