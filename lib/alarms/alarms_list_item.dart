import 'package:flutter/material.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/theme.dart';

class AlarmListItem extends StatelessWidget {
  final Alarm alarm;
  const AlarmListItem({Key? key, required this.alarm}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.only(bottom: 10),
      decoration: BoxDecoration(
        border: Border.all(
          width: 3,
          color: appTheme.backgroundColor,
        ),
        borderRadius: const BorderRadius.all(
          Radius.circular(8),
        ),
        color: appTheme.cardColor,
      ),
      child: InkWell(
        onTap: () {
          // Navigator.of(context).push(
          //   MaterialPageRoute(
          //     builder: (BuildContext context) => AlarmScreen(alarm: alarm),
          //   ),
          // );
        },
        child: Row(
          children: [
            Flexible(
              child: Padding(
                padding: const EdgeInsets.only(left: 15, top: 20, bottom: 20),
                child: Text(
                  alarm.alarmName,
                  overflow: TextOverflow.fade,
                  softWrap: false,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
