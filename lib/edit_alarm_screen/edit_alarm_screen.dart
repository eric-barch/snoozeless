// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/edit_alarm_screen/edit_alarm_form.dart';

class EditAlarmScreen extends StatelessWidget {
  final String alarmId;
  const EditAlarmScreen({Key? key, required this.alarmId}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Edit Alarm'),
      ),
      body: EditAlarmForm(alarmId: alarmId),
    );
  }
}
